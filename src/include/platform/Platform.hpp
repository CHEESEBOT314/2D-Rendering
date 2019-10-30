#ifndef MSCFINALPROJECT_PLATFORM_PLATFORM_HPP
#define MSCFINALPROJECT_PLATFORM_PLATFORM_HPP

#include <string>

namespace Platform {
    namespace Files {
        std::string getResourceFolder();
        void createFolder(const std::string& folder);
    }
}

#endif//MSCFINALPROJECT_PLATFORM_PLATFORM_HPP