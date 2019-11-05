#include "render/RenderManager.hpp"

#include "VulkanWrapper.hpp"
#include "render/Vertex.hpp"
#include "resource/ResourceManager.hpp"

#include <map>

namespace render::RenderManager {
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

            bool loadPipeline(const std::string &name, Pipeline &pipeline) {
                vk::ShaderModule vert, frag;
                if (!VulkanWrapper::createShaderModule(vert, resource::ResourceManager::readBinaryFile(name + ".vs.spv",
                                                                                             {"shaders"})) ||
                    !VulkanWrapper::createShaderModule(frag, resource::ResourceManager::readBinaryFile(name + ".fs.spv",
                                                                                             {"shaders"}))) {
                    VulkanWrapper::destroyShaderModule(vert);
                    VulkanWrapper::destroyShaderModule(frag);
                    return false;
                }
                vk::PipelineShaderStageCreateInfo shaderStageCreateInfos[2];
                shaderStageCreateInfos[0] = vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                                                              vk::ShaderStageFlagBits::eVertex, vert,
                                                                              "main");
                shaderStageCreateInfos[1] = vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                                                              vk::ShaderStageFlagBits::eFragment, frag,
                                                                              "main");

                vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {vk::PipelineLayoutCreateFlags(),
                                                                         0,
                                                                         nullptr,
                                                                         0,
                                                                         nullptr};
                if (!VulkanWrapper::createPipelineLayout(pipeline.layout, pipelineLayoutCreateInfo)) {
                    VulkanWrapper::destroyShaderModule(vert);
                    VulkanWrapper::destroyShaderModule(frag);
                    return false;
                }
                vk::VertexInputBindingDescription vertexInputBindingDescription = {0,
                                                                                   sizeof(Vertex),
                                                                                   vk::VertexInputRate::eVertex};
                vk::VertexInputAttributeDescription vertexInputAttributeDescriptions[2];
                vertexInputAttributeDescriptions[0] = {0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, pos)};
                vertexInputAttributeDescriptions[1] = {1, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)};

                if (!VulkanWrapper::createPipeline(pipeline.pl, pipeline.layout, 2, shaderStageCreateInfos, 1, &vertexInputBindingDescription, 2, vertexInputAttributeDescriptions)) {
                    VulkanWrapper::destroyShaderModule(vert);
                    VulkanWrapper::destroyShaderModule(frag);
                    VulkanWrapper::destroyPipelineLayout(pipeline.layout);
                    return false;
                }

                VulkanWrapper::destroyShaderModule(vert);
                VulkanWrapper::destroyShaderModule(frag);

                printf("Successfully loaded '%s' shader\n", name.c_str());
                return true;
            }
        }

        void init() {
            info = std::make_unique<Info>();
        }

        bool createGraphicsPipeline(const std::string &name) {
            info->nameToIdMap.insert(std::pair<const std::string, uint32_t>(name, info->nextId));
            info->nextId++;
            if (info->loaded) {
                Pipeline pipeline;
                if (!loadPipeline(name, pipeline)) {
                    return false;
                }
                info->idToPipelineMap.insert(std::pair<const uint32_t, Pipeline>(info->nextId - 1, pipeline));
            }
            return true;
        }

        uint32_t getPipelineID(const std::string &name) {
            auto it = info->nameToIdMap.find(name);
            if (it != info->nameToIdMap.end()) {
                return it->second;
            }
            return 0;
        }

        void bindPipeline(uint32_t id) {
            if (id > 0) {
                auto it = info->idToPipelineMap.find(id);
                if (it != info->idToPipelineMap.end()) {
                    VulkanWrapper::bindPipeline(it->second.pl);
                }
            }
        }

        void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
            VulkanWrapper::draw(vertexCount, instanceCount, firstVertex, firstInstance);
        }

        bool loadShaders() {
            for (const std::pair<const std::string, uint32_t>& nPair : info->nameToIdMap) {
                Pipeline pipeline;
                if (!loadPipeline(nPair.first, pipeline)) {
                    unloadShaders();
                    return false;
                }
                info->idToPipelineMap.insert(std::pair<const uint32_t, Pipeline>(nPair.second, pipeline));
            }
            return (info->loaded = true);
        }

        void unloadShaders() {
            for (const std::pair<const uint32_t, Pipeline>& pPair : info->idToPipelineMap) {
                VulkanWrapper::destroyPipelineLayout(pPair.second.layout);
                VulkanWrapper::destroyPipeline(pPair.second.pl);
            }
            info->idToPipelineMap.clear();
            info->loaded = false;
        }

        bool reloadShaders() {
            unloadShaders();
            return loadShaders();
        }

        void terminate() {
            if (info->loaded) {
                unloadShaders();
            }
            info->nameToIdMap.clear();
            info.reset(nullptr);
        }
    }