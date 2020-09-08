#include "platform/platform.hpp"

#include <CoreServices/CoreServices.h>

namespace platform::files {
    const char FILE_SEPARATOR = '/';
    std::string get_resource_folder() {
        CFBundleRef main_bundle = CFBundleGetMainBundle();
        CFURLRef base = CFBundleCopySupportFilesDirectoryURL(main_bundle);
        char path[PATH_MAX];

        CFURLGetFileSystemRepresentation(base, true, (uint8_t*)path, PATH_MAX);
        CFRelease(base);

        return std::string(path) + "/Resources/";
    }
    void create_folder(const std::string& folder) {
    }
}