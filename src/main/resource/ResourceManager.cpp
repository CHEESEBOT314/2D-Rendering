#include "resource/ResourceManager.hpp"

#include <memory>
#include <fstream>

namespace resource::ResourceManager {
    namespace {
        struct Info {
            std::string folder;
            char separator = 0;
        };
        std::unique_ptr<Info> info;
    }
    void init(const std::string& folder, char separator) {
        info = std::make_unique<Info>();
        info->folder = folder;
        info->separator = separator;
    }
    std::vector<uint8_t> readBinaryFile(const std::string& fileName, const std::vector<std::string>& folders) {
        std::string fullPath = info->folder;
        for (const std::string& d : folders) {
            fullPath = fullPath.append(d);
            fullPath = fullPath.append(&info->separator, 1);
        }
        fullPath = fullPath.append(fileName);
        printf("Reading file: %s\n", fullPath.c_str());
        std::ifstream file(fullPath, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            return {};
        }
        size_t fileSize = (size_t) file.tellg();
        std::vector<uint8_t> buffer(fileSize);
        file.seekg(0);
        file.read((char*)buffer.data(), fileSize);
        return buffer;
    }
}