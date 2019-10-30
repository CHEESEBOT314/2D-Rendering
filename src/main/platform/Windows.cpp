#include "platform/Platform.hpp"

#include "Windows.h"

namespace Platform {
    namespace Files {
        std::string getResourceFolder() {
            HMODULE hModule = GetModuleHandle(NULL);
            char path[MAX_PATH];
            GetModuleFilename(NULL, path, sizeof(path));
            return std::string(path);
        }
        void createFolder(const std::string& folder) {

        }
    }
}