#include <render/SpriteManager.hpp>

#include <memory>
#include <map>
#include <vml/mat3.hpp>
#include <vector>
#include <render/RenderManager.hpp>
#include <resource/ResourceManager.hpp>

namespace render::SpriteManager {
    namespace {
        struct Info {
            std::map<std::string, uint32_t> nameToIdMap;
            std::map<uint32_t, vml::mat3> idToTransformMap;

            uint32_t unknownID;

            float width;
            float height;
            float layers;
        };
        std::unique_ptr<Info> info;

        uint32_t convertEndian(const uint8_t* in) {
            uint32_t out = 0;
            out += ((uint32_t)in[0]) << 24;
            out += ((uint32_t)in[1]) << 16;
            out += ((uint32_t)in[2]) << 8;
            out += ((uint32_t)in[3]) << 0;
            return out;
        }
        vml::mat3 constructTransform(const uint8_t* data) {
            return vml::mat3(
                    convertEndian(data) / info->width, 0.0f, 0.0f,
                    0.0f, convertEndian(data + 4) / info->height, 0.0f,
                    convertEndian(data + 8) / info->width, convertEndian(data + 12) / info->height, convertEndian(data + 16) / info->layers);
        }
    }
    bool init() {
        info = std::make_unique<Info>();

        std::vector<uint8_t> descriptionData = resource::ResourceManager::readBinaryFile("sprites.ats", {"textures"});
        uint8_t* ptr = descriptionData.data();

        uint32_t spriteCount = ((uint32_t*)ptr)[0];
        ptr += 4;
        for (uint32_t i = 0; i < spriteCount; i++) {
            info->idToTransformMap.insert(std::pair<uint32_t, vml::mat3>(i + 1, constructTransform(ptr)));
            ptr += 20;
            uint32_t charCount = ((uint32_t*)ptr)[0];
            ptr += 4;
            info->nameToIdMap.insert(std::pair<std::string, uint32_t>(std::string((char*)ptr, charCount), i + 1));
            ptr += charCount;
            if (*ptr != 0) {
                info.reset(nullptr);
                return false;
            }
            ptr += 1;
        }
        if (*ptr != 0) {
            info.reset(nullptr);
            return false;
        }
        info->unknownID = getID("unknown");
        return true;
    }
    uint32_t getID(const std::string& name) {
        auto it = info->nameToIdMap.find(name);
        if (it != info->nameToIdMap.end()) {
            return it->second;
        }
        return 0;
    }
    void bindSprite(uint32_t id) {
        if (id > 0) {
            auto it = info->idToTransformMap.find(id);
            if (it != info->idToTransformMap.end()) {

            }
        }
    }
}