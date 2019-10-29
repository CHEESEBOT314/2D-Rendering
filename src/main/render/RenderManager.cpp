#include "render/RenderManager.hpp"

#include "VulkanWrapper.hpp"

#include <map>

namespace RenderManager {
    namespace {
        struct Pipeline {
            vk::PipelineLayout layout;
            vk::Pipeline pl;
        };

        struct Info {
            std::map<std::string, uint32_t> nameToIdMap;
            std::map<uint32_t, Pipeline> idToPipelineMap;
            uint32_t nextId;
            bool loaded = false;
            bool handled = false;
        };
        std::unique_ptr<Info> info;

        bool loadPipeline(const std::string& name, Pipeline& pipeline) {

            return true;
        }
    }

    bool createGraphicsPipeline(const std::string& name) {
        return true;
    }
    uint32_t getPipelineID(const std::string& name) {
        return 0;
    }

    void bindPipeline(uint32_t id) {

    }

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {

    }

    bool loadShaders() {
        return true;
    }
    void unloadShaders() {

    }
    bool reloadShaders() {
        unloadShaders();
        return loadShaders();
    }
}