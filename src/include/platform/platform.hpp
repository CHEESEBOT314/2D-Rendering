#ifndef MSCFINALPROJECT_PLATFORM_PLATFORM_HPP
#define MSCFINALPROJECT_PLATFORM_PLATFORM_HPP

#include <string>

namespace platform {
    namespace files {
        extern const char FILE_SEPARATOR;
        std::string get_resource_folder();
        void create_folder(const std::string& folder);
    }
}

#endif//MSCFINALPROJECT_PLATFORM_PLATFORM_HPP