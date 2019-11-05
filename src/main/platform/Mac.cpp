#include "platform/Platform.hpp"

#include <CoreServices/CoreServices.h>

namespace Platform::Files {
    const char FILE_SEPARATOR = '/';
    std::string getResourceFolder() {
        CFBundleRef main_bundle = CFBundleGetMainBundle();
        CFURLRef base = CFBundleCopySupportFilesDirectoryURL(main_bundle);
        char path[PATH_MAX];

        CFURLGetFileSystemRepresentation(base, true, (uint8_t*)path, PATH_MAX);
        CFRelease(base);

        return std::string(path) + "/Resources/";
    }
    void createFolder(const std::string& folder) {
    }
}