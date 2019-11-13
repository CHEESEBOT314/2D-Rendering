#include "render/RenderManager.hpp"

#include "VulkanWrapper.hpp"
#include "render/PushConstants.hpp"
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

                vk::Buffer rect2D;
                vk::DeviceMemory rect2DMemory;
                vk::DeviceSize* offsets = nullptr;

                PushConstants currentPC;
                Pipeline* currentPL = nullptr;
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

                vk::PushConstantRange pushConstantRange = {vk::ShaderStageFlagBits::eVertex,
                                                           0,
                                                           sizeof(PushConstants)};

                vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {vk::PipelineLayoutCreateFlags(),
                                                                         0,
                                                                         nullptr,
                                                                         1,
                                                                         &pushConstantRange};
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

                if (!VulkanWrapper::createPipeline(pipeline.pl, pipeline.layout, 2, shaderStageCreateInfos, 1, &vertexInputBindingDescription, 2, vertexInputAttributeDescriptions, 1.0f)) {
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
            static const std::vector<Vertex> vertices2D = {
                    {{0.0f, 0.0f}, {0.0f, 0.0f}},
                    {{1.0f, 0.0f}, {1.0f, 0.0f}},
                    {{1.0f, 1.0f}, {1.0f, 1.0f}},
                    {{0.0f, 0.0f}, {0.0f, 0.0f}},
                    {{1.0f, 1.0f}, {1.0f, 1.0f}},
                    {{0.0f, 1.0f}, {0.0f, 1.0f}}};
            VulkanWrapper::createVertexBuffer(info->rect2D, info->rect2DMemory, sizeof(Vertex) * vertices2D.size());
            VulkanWrapper::mapVertexBuffer(info->rect2DMemory, sizeof(Vertex) * vertices2D.size(), vertices2D.data());
            info->offsets = new vk::DeviceSize[1]{0};
            resetPushConstants();
        }

        bool createGraphicsPipeline(const std::string &name) {
            info->nameToIdMap.insert(std::pair<const std::string, uint32_t>(name, info->nextId));
            if (info->loaded) {
                Pipeline pipeline;
                if (!loadPipeline(name, pipeline)) {
                    return false;
                }
                info->idToPipelineMap.insert(std::pair<const uint32_t, Pipeline>(info->nextId, pipeline));
            }
            info->nextId++;
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
                    info->currentPL = &it->second;
                }
            }
        }

        void resetPushConstants() {
            info->currentPC.p = vml::mat4::identity();
            info->currentPC.v = vml::mat4::identity();
            info->currentPC.m = vml::mat4::identity();
            info->currentPC.textureTransform = vml::mat3(1.0f, 0.0f, 0.0,
                                                       0.0f, 1.0f, 0.0f,
                                                       0.0f, 0.0f, 0.0f);
        }
        void setPerspective(const vml::mat4& pers) {
            info->currentPC.p = pers;
        }
        void setView(const vml::mat4& view) {
            info->currentPC.v = view;
        }
        void setModel(const vml::mat4& mode) {
            info->currentPC.m = mode;
        }
        void setTextureTransform(const vml::mat3& tt) {
            info->currentPC.textureTransform = tt;
        }

        void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
            VulkanWrapper::pushConstants(info->currentPL->layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstants), &info->currentPC);
            VulkanWrapper::draw(vertexCount, instanceCount, firstVertex, firstInstance);
        }
        void drawRect2D() {
            VulkanWrapper::bindVertexBuffers(1, &info->rect2D, info->offsets);
            draw(6, 1, 0, 0);
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
            delete[] info->offsets;
            VulkanWrapper::destroyVertexBuffer(info->rect2D, info->rect2DMemory);
            info->nameToIdMap.clear();
            info.reset(nullptr);
        }
    }