#include "morphgeometry.hpp"

#include <cassert>
#include <components/resource/scenemanager.hpp>

namespace SceneUtil
{

MorphGeometry::MorphGeometry()
    : mLastFrameNumber(0)
    , mDirty(true)
    , mMorphedBoundingBox(false)
{

}

MorphGeometry::MorphGeometry(const MorphGeometry &copy, const osg::CopyOp &copyop)
    : osg::Drawable(copy, copyop)
    , mMorphTargets(copy.mMorphTargets)
    , mLastFrameNumber(0)
    , mDirty(true)
    , mMorphedBoundingBox(false)
{
    setSourceGeometry(copy.getSourceGeometry());
}

void MorphGeometry::setSourceGeometry(osg::ref_ptr<osg::Geometry> sourceGeom)
{
    for (unsigned int i=0; i<2; ++i)
        mGeometry[i] = nullptr;

    mSourceGeometry = sourceGeom;

    for (unsigned int i=0; i<2; ++i)
    {
        // DO NOT COPY AND PASTE THIS CODE. Cloning osg::Geometry without also cloning its contained Arrays is generally unsafe.
        // In this specific case the operation is safe under the following two assumptions:
        // - When Arrays are removed or replaced in the cloned geometry, the original Arrays in their place must outlive the cloned geometry regardless. (ensured by TemplateRef)
        // - Arrays that we add or replace in the cloned geometry must be explicitely forbidden from reusing BufferObjects of the original geometry. (ensured by vbo below)
        mGeometry[i] = new osg::Geometry(*mSourceGeometry, osg::CopyOp::SHALLOW_COPY);
        mGeometry[i]->getOrCreateUserDataContainer()->addUserObject(new Resource::TemplateRef(mSourceGeometry));

        const osg::Geometry& from = *mSourceGeometry;
        osg::Geometry& to = *mGeometry[i];
        to.setSupportsDisplayList(false);
        to.setUseVertexBufferObjects(true);
        to.setCullingActive(false); // make sure to disable culling since that's handled by this class

        // vertices are modified every frame, so we need to deep copy them.
        // assign a dedicated VBO to make sure that modifications don't interfere with source geometry's VBO.
        osg::ref_ptr<osg::VertexBufferObject> vbo (new osg::VertexBufferObject);
        vbo->setUsage(GL_DYNAMIC_DRAW_ARB);

        osg::ref_ptr<osg::Array> vertexArray = static_cast<osg::Array*>(from.getVertexArray()->clone(osg::CopyOp::DEEP_COPY_ALL));
        if (vertexArray)
        {
            vertexArray->setVertexBufferObject(vbo);
            to.setVertexArray(vertexArray);
        }
    }
}

void MorphGeometry::addMorphTarget(osg::Vec3Array *offsets, float weight)
{
    mMorphTargets.push_back(MorphTarget(offsets, weight));
    mMorphedBoundingBox = false;
    dirty();
}

void MorphGeometry::dirty()
{
    mDirty = true;
    if (!mMorphedBoundingBox)
        dirtyBound();
}

osg::ref_ptr<osg::Geometry> MorphGeometry::getSourceGeometry() const
{
    return mSourceGeometry;
}

void MorphGeometry::accept(osg::NodeVisitor &nv)
{
    if (!nv.validNodeMask(*this))
        return;

    nv.pushOntoNodePath(this);

    if (nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
        cull(&nv);
    else
        nv.apply(*this);

    nv.popFromNodePath();
}

void MorphGeometry::accept(osg::PrimitiveFunctor& func) const
{
    getGeometry(mLastFrameNumber)->accept(func);
}

osg::BoundingBox MorphGeometry::computeBoundingBox() const
{
    bool anyMorphTarget = false;
    for (unsigned int i=1; i<mMorphTargets.size(); ++i)
        if (mMorphTargets[i].getWeight() != 0)
        {
            anyMorphTarget = true;
            break;
        }

    // before the MorphGeometry has started animating, we will use a regular bounding box (this is required
    // for correct object placements, which uses the bounding box)
    if (!mMorphedBoundingBox && !anyMorphTarget)
    {
        return mSourceGeometry->getBoundingBox();
    }
    // once it animates, use a bounding box that encompasses all possible animations so as to avoid recalculating
    else
    {
        mMorphedBoundingBox = true;

        const osg::Vec3Array* sourceVerts = static_cast<const osg::Vec3Array*>(mSourceGeometry->getVertexArray());
        if (mMorphTargets.size() != 0)
            sourceVerts = mMorphTargets[0].getOffsets();
        std::vector<osg::BoundingBox> vertBounds(sourceVerts->size());

        // Since we don't know what combinations of morphs are being applied we need to keep track of a bounding box for each vertex.
        // The minimum/maximum of the box is the minimum/maximum offset the vertex can have from its starting position.

        // Start with zero offsets which will happen when no morphs are applied.
        for (unsigned int i=0; i<vertBounds.size(); ++i)
            vertBounds[i].set(osg::Vec3f(0,0,0), osg::Vec3f(0,0,0));

        for (unsigned int i = 1; i < mMorphTargets.size(); ++i)
        {
            const osg::Vec3Array& offsets = *mMorphTargets[i].getOffsets();
            for (unsigned int j=0; j<offsets.size() && j<vertBounds.size(); ++j)
            {
                osg::BoundingBox& bounds = vertBounds[j];
                bounds.expandBy(bounds._max + offsets[j]);
                bounds.expandBy(bounds._min + offsets[j]);
            }
        }

        osg::BoundingBox box;
        for (unsigned int i=0; i<vertBounds.size(); ++i)
        {
            vertBounds[i]._max += (*sourceVerts)[i];
            vertBounds[i]._min += (*sourceVerts)[i];
            box.expandBy(vertBounds[i]);
        }
        return box;
    }
}

void MorphGeometry::cull(osg::NodeVisitor *nv)
{
    if (mLastFrameNumber == nv->getTraversalNumber() || !mDirty || mMorphTargets.size() == 0)
    {
        osg::Geometry& geom = *getGeometry(mLastFrameNumber);
        nv->pushOntoNodePath(&geom);
        nv->apply(geom);
        nv->popFromNodePath();
        return;
    }

    mDirty = false;
    mLastFrameNumber = nv->getTraversalNumber();
    osg::Geometry& geom = *getGeometry(mLastFrameNumber);

    const osg::Vec3Array* positionSrc = mMorphTargets[0].getOffsets();
    osg::Vec3Array* positionDst = static_cast<osg::Vec3Array*>(geom.getVertexArray());
    assert(positionSrc->size() == positionDst->size());
    for (unsigned int vertex=0; vertex<positionSrc->size(); ++vertex)
        (*positionDst)[vertex] = (*positionSrc)[vertex];

    for (unsigned int i=1; i<mMorphTargets.size(); ++i)
    {
        float weight = mMorphTargets[i].getWeight();
        if (weight == 0.f)
            continue;
        const osg::Vec3Array* offsets = mMorphTargets[i].getOffsets();
        for (unsigned int vertex=0; vertex<positionSrc->size(); ++vertex)
            (*positionDst)[vertex] += (*offsets)[vertex] * weight;
    }

    positionDst->dirty();

    geom.osg::Drawable::dirtyGLObjects();

    nv->pushOntoNodePath(&geom);
    nv->apply(geom);
    nv->popFromNodePath();
}

osg::Geometry* MorphGeometry::getGeometry(unsigned int frame) const
{
    return mGeometry[frame%2];
}


}
