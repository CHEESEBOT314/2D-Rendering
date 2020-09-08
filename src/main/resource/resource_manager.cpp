#include "resource/resource_manager.hpp"

#include <memory>
#include <fstream>

namespace resource::resource_manager {
    namespace {
        struct info {
            std::string folder;
            char separator = 0;
        };
        std::unique_ptr<info> info_p;
    }
    void init(const std::string& folder, char separator) {
        info_p = std::make_unique<info>();
        info_p->folder = folder;
        info_p->separator = separator;
    }
    std::vector<uint8_t> read_binary_file(const std::string& file_name, const std::vector<std::string>& folders) {
        std::string fullPath = info_p->folder;
        for (const std::string& d : folders) {
            fullPath = fullPath.append(d);
            fullPath = fullPath.append(&info_p->separator, 1);
        }
        fullPath = fullPath.append(file_name);
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