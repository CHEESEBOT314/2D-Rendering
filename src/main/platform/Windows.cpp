#include "platform/platform.hpp"

#include "windows.h"

namespace Platform::Files {
    const char FILE_SEPARATOR = '\\';
    std::string getResourceFolder() {
        HMODULE hModule = GetModuleHandle(nullptr);
        char path[MAX_PATH];
        GetModuleFileName(nullptr, path, sizeof(path));
        std::string pathStr = std::string(path);
        return pathStr.substr(0, pathStr.find_last_of('\\') + 1).append("resources\\");
    }
    void createFolder(const std::string& folder) {
    }
}