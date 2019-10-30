#ifndef MSCFINALPROJECT_VULKAN_WRAPPER_H
#define MSCFINALPROJECT_VULKAN_WRAPPER_H

#include "vulkan/vulkan.hpp"

namespace VulkanWrapper {

    bool createInstance(std::vector<const char*> extensions);
    bool createSurface(bool(*fn)(vk::Instance, vk::SurfaceKHR*), void (*r)(int*, int*));
    bool createOthers();
    bool createSwapchain();

    bool createShaderModule(vk::ShaderModule& shaderModule, const std::vector<uint8_t>& src);
    void destroyShaderModule(const vk::ShaderModule& shaderModule);

    bool createPipelineLayout();
    void destroyPipelineLayout();
    bool createPipeline();
    void destroyPipeline();

    bool renderFrame(void (*externalRender)());

    void bindPipeline(const vk::Pipeline& pipeline);
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

    bool reloadSwapchain();
    void destroySwapchain();
    void terminate();
}

#endif//MSCFINALPROJECT_VULKAN_WRAPPER_H
