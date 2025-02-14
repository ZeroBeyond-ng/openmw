#include "collections.hpp"

#include <components/misc/strings/algorithm.hpp>
#include <components/misc/strings/lower.hpp>

namespace Files
{
    Collections::Collections()
        : mDirectories()
        , mFoldCase(false)
        , mCollections()
    {
    }

    Collections::Collections(const Files::PathContainer& directories, bool foldCase)
        : mDirectories(directories)
        , mFoldCase(foldCase)
        , mCollections()
    {
    }

    const MultiDirCollection& Collections::getCollection(const std::string& extension) const
    {
        std::string ext = Misc::StringUtils::lowerCase(extension);
        auto iter = mCollections.find(ext);
        if (iter==mCollections.end())
        {
            std::pair<MultiDirCollectionContainer::iterator, bool> result =
                mCollections.emplace(ext, MultiDirCollection(mDirectories, ext, mFoldCase));

            iter = result.first;
        }

        return iter->second;
    }

    boost::filesystem::path Collections::getPath(const std::string& file) const
    {
        for (Files::PathContainer::const_iterator iter = mDirectories.begin();
             iter != mDirectories.end(); ++iter)
        {
            for (boost::filesystem::directory_iterator iter2 (*iter);
                iter2!=boost::filesystem::directory_iterator(); ++iter2)
            {
                boost::filesystem::path path = *iter2;

                if (mFoldCase)
                {
                    if (Misc::StringUtils::ciEqual(file, path.filename().string()))
                        return path.string();
                }
                else if (path.filename().string() == file)
                    return path.string();
            }
        }

        throw std::runtime_error ("file " + file + " not found");
    }

    bool Collections::doesExist(const std::string& file) const
    {
        for (Files::PathContainer::const_iterator iter = mDirectories.begin();
             iter != mDirectories.end(); ++iter)
        {
            for (boost::filesystem::directory_iterator iter2 (*iter);
                iter2!=boost::filesystem::directory_iterator(); ++iter2)
            {
                boost::filesystem::path path = *iter2;

                if (mFoldCase)
                {
                    if (Misc::StringUtils::ciEqual(file, path.filename().string()))
                        return true;
                }
                else if (path.filename().string() == file)
                    return true;
            }
        }

        return false;
    }

    const Files::PathContainer& Collections::getPaths() const
    {
        return mDirectories;
    }
}
