#ifndef CSM_WOLRD_RESOURCESMANAGER_H
#define CSM_WOLRD_RESOURCESMANAGER_H

#include <map>

#include "universalid.hpp"

namespace VFS
{
    class Manager;
}

namespace CSMWorld
{
    class Resources;

    class ResourcesManager
    {
            std::map<UniversalId::Type, Resources> mResources;
            const VFS::Manager* mVFS;

        private:

            void addResources (const Resources& resources);

            const char * const * getMeshExtensions();

        public:

            ResourcesManager();
            ~ResourcesManager();

            const VFS::Manager* getVFS() const;

            void setVFS(const VFS::Manager* vfs);

            void recreateResources();

            const Resources& get (UniversalId::Type type) const;
    };
}

#endif
