#include "render/SpriteManager.hpp"

#include <memory>
#include <map>
#include <vml/mat3.hpp>
#include <vector>
#include <resource/ResourceManager.hpp>

namespace render::SpriteManager {
    namespace {
        struct Info {
            std::map<std::string, uint32_t> nameToIdMap;
            std::map<uint32_t, vml::mat3> idToTransformMap;
        };
        std::unique_ptr<Info> info;

        vml::mat3 constructTransform(float* data) {
            return vml::mat3(
                    data[0], 0.0f,    0.0f,
                    0.0f,    data[1], 0.0f,
                    data[2], data[3], data[4]);
        }
    }
    void init() {
        info = std::make_unique<Info>();

        std::vector<uint8_t> descriptionData = resource::ResourceManager::readBinaryFile("sprites.sadf", {"textures"});
        uint8_t* ptr = descriptionData.data();
        if (ptr[0] != 's' || ptr[1] != 'a' || ptr[2] != 'd' || ptr[3] != 'f') {
            info.reset(nullptr);
            return;
        }
        ptr += 4;
        uint32_t spriteCount = ((uint32_t*)ptr)[0];
        ptr += 4;
        for (uint32_t i = 0; i < spriteCount; i++) {
            info->idToTransformMap.insert(std::pair<uint32_t, vml::mat3>(i + 1, constructTransform((float*)ptr)));
            ptr += 20;
            uint32_t charCount = ((uint32_t*)ptr)[0];
            ptr += 4;
            info->nameToIdMap.insert(std::pair<std::string, uint32_t>(std::string((char*)ptr, charCount), i + 1));
            ptr += charCount;
            if (*ptr != 0) {
                info.reset(nullptr);
                return;
            }
            ptr += 1;
        }
        if (*ptr != 0) {
            info.reset(nullptr);
        }
    }
}