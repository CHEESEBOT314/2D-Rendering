#ifndef MSCFINALPROJECT_RESOURCE_RESOURCEMANAGER_HPP
#define MSCFINALPROJECT_RESOURCE_RESOURCEMANAGER_HPP

#include <string>
#include <vector>

namespace resource::resource_manager {
    void init(const std::string& folder, char separator);
    std::vector<uint8_t> read_binary_file(const std::string& file_name, const std::vector<std::string>& folders);
}

#endif//MSCFINALPROJECT_RESOURCEMANAGER_HPP
