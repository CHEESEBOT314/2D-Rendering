#ifndef MSCFINALPROJECT_RESOURCE_RESOURCEMANAGER_HPP
#define MSCFINALPROJECT_RESOURCE_RESOURCEMANAGER_HPP

#include <string>
#include <vector>

namespace resource::ResourceManager {
    void init(const std::string& folder, char separator);
    std::vector<uint8_t> readBinaryFile(const std::string& fileName, const std::vector<std::string>& folders);
}

#endif//MSCFINALPROJECT_RESOURCEMANAGER_HPP
