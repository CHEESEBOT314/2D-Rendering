#include "render/RenderManager.hpp"

#include "VulkanWrapper.hpp"
#include "resource/ResourceManager.hpp"

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
            uint32_t nextId = 1;
            bool loaded = false;
        };
        std::unique_ptr<Info> info;

        bool loadPipeline(const std::string& name, Pipeline& pipeline) {
            vk::ShaderModule vert, frag;
            if (!VulkanWrapper::createShaderModule(vert, ResourceManager::readBinaryFile(name + ".vs.spv", {"shaders"})) ||
                !VulkanWrapper::createShaderModule(frag, ResourceManager::readBinaryFile(name + ".fs.spv", {"shaders"}))) {
                return false;
            }
            vk::PipelineShaderStageCreateInfo shaderStageCreateInfos[2];
            shaderStageCreateInfos[0] = vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vert, "main");
            shaderStageCreateInfos[1] = vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, frag, "main");


            return true;
        }
    }
    void init() {
        info = std::make_unique<Info>();

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