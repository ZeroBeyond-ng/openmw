#ifndef OPENMW_COMPONENTS_DETOURNAVIGATOR_NAVIGATORSTUB_H
#define OPENMW_COMPONENTS_DETOURNAVIGATOR_NAVIGATORSTUB_H

#include "navigator.hpp"
#include "settings.hpp"
#include "stats.hpp"

namespace Loading
{
    class Listener;
}

namespace DetourNavigator
{
    class NavigatorStub final : public Navigator
    {
    public:
        NavigatorStub() = default;

        void addAgent(const AgentBounds& /*agentBounds*/) override {}

        void removeAgent(const AgentBounds& /*agentBounds*/) override {}

        void setWorldspace(std::string_view /*worldspace*/) override {}

        void addObject(const ObjectId /*id*/, const ObjectShapes& /*shapes*/, const btTransform& /*transform*/) override {}

        void addObject(const ObjectId /*id*/, const DoorShapes& /*shapes*/, const btTransform& /*transform*/) override {}

        void updateObject(const ObjectId /*id*/, const ObjectShapes& /*shapes*/, const btTransform& /*transform*/) override {}

        void updateObject(const ObjectId /*id*/, const DoorShapes& /*shapes*/, const btTransform& /*transform*/) override {}

        void removeObject(const ObjectId /*id*/) override {}

        void addWater(const osg::Vec2i& /*cellPosition*/, int /*cellSize*/, float /*level*/) override {}

        void removeWater(const osg::Vec2i& /*cellPosition*/) override {}

        void addHeightfield(const osg::Vec2i& /*cellPosition*/, int /*cellSize*/, const HeightfieldShape& /*height*/)
            override {}

        void removeHeightfield(const osg::Vec2i& /*cellPosition*/) override {}

        void addPathgrid(const ESM::Cell& /*cell*/, const ESM::Pathgrid& /*pathgrid*/) override {}

        void removePathgrid(const ESM::Pathgrid& /*pathgrid*/) override {}

        void update(const osg::Vec3f& /*playerPosition*/) override {}

        void updateBounds(const osg::Vec3f& /*playerPosition*/) override {}

        void wait(Loading::Listener& /*listener*/, WaitConditionType /*waitConditionType*/) override {}

        SharedNavMeshCacheItem getNavMesh(const AgentBounds& /*agentBounds*/) const override
        {
            return mEmptyNavMeshCacheItem;
        }

        std::map<AgentBounds, SharedNavMeshCacheItem> getNavMeshes() const override
        {
            return {};
        }

        const Settings& getSettings() const override
        {
            return mDefaultSettings;
        }

        Stats getStats() const override { return Stats {}; }

        RecastMeshTiles getRecastMeshTiles() const override
        {
            return {};
        }

        float getMaxNavmeshAreaRealRadius() const override
        {
            return std::numeric_limits<float>::max();
        }

    private:
        Settings mDefaultSettings {};
        SharedNavMeshCacheItem mEmptyNavMeshCacheItem;
    };
}

#endif
