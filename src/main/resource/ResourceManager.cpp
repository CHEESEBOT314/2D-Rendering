#include "resource/ResourceManager.hpp"

namespace ResourceManager {
    namespace {
        struct Info {
            std::string folder;
        };
        std::unique_ptr<Info> info;
    }
    void init(const std::string& folder) {
        info = std::make_unique<Info>();
        info->folder = folder;
    }
    std::vector<uint8_t> readBinaryFile(const std::string& fileName) {
        std::ifstream file(info->folder + "/" + fileName, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            return {};
        }
        size_t fileSize = (size_t) file.tellg();
        std::vector<uint8_t> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        return buffer;
    }
}