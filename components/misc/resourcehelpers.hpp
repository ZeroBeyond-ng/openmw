#ifndef MISC_RESOURCEHELPERS_H
#define MISC_RESOURCEHELPERS_H

#include <string>
#include <string_view>

namespace VFS
{
    class Manager;
}

namespace Misc
{
    // Workarounds for messy resource handling in vanilla morrowind
    // The below functions are provided on a opt-in basis, instead of built into the VFS,
    // so we have the opportunity to use proper resource handling for content created in OpenMW-CS.
    namespace ResourceHelpers
    {
        bool changeExtensionToDds(std::string &path);
        std::string correctResourcePath(std::string_view topLevelDirectory, std::string_view resPath, const VFS::Manager* vfs);
        std::string correctTexturePath(std::string_view resPath, const VFS::Manager* vfs);
        std::string correctIconPath(std::string_view resPath, const VFS::Manager* vfs);
        std::string correctBookartPath(std::string_view resPath, const VFS::Manager* vfs);
        std::string correctBookartPath(std::string_view resPath, int width, int height, const VFS::Manager* vfs);
        /// Use "xfoo.nif" instead of "foo.nif" if available
        std::string correctActorModelPath(const std::string &resPath, const VFS::Manager* vfs);
        std::string correctMeshPath(const std::string &resPath, const VFS::Manager* vfs);

        std::string correctSoundPath(std::string_view resPath, const VFS::Manager* vfs);

        /// marker objects that have a hardcoded function in the game logic, should be hidden from the player
        bool isHiddenMarker(std::string_view id);
    }
}

#endif
