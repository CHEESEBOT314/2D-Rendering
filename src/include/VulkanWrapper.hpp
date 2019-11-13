#ifndef MSCFINALPROJECT_VULKAN_WRAPPER_H
#define MSCFINALPROJECT_VULKAN_WRAPPER_H

#include "vulkan/vulkan.hpp"

namespace VulkanWrapper {

    bool createInstance(std::vector<const char*> extensions);
    bool createSurface(bool(*fn)(const vk::Instance&, vk::SurfaceKHR&), void (*r)(int*, int*));
    bool createOthers();
    bool createSwapchain();

    bool createVertexBuffer(vk::Buffer& buffer, vk::DeviceMemory& memory, uint32_t size);
    void mapVertexBuffer(const vk::DeviceMemory& memory, uint32_t size, const void* data);
    void destroyVertexBuffer(const vk::Buffer& buffer, const vk::DeviceMemory& memory);

    bool createShaderModule(vk::ShaderModule& shaderModule, const std::vector<uint8_t>& src);
    void destroyShaderModule(const vk::ShaderModule& shaderModule);

    bool createPipelineLayout(vk::PipelineLayout& pipelineLayout, const vk::PipelineLayoutCreateInfo& pipelineLayoutCreateInfo);
    void destroyPipelineLayout(const vk::PipelineLayout& pipelineLayout);
    bool createPipeline(vk::Pipeline& pipeline, const vk::PipelineLayout& pipelineLayout, uint32_t shaderModuleCount, const vk::PipelineShaderStageCreateInfo* shaderModules, uint32_t vertexBindingDescriptionCount, const vk::VertexInputBindingDescription* vertexBindingDescriptions, uint32_t vertexAttributeDescriptionCount, const vk::VertexInputAttributeDescription* vertexAttributeDescriptions, float targetAspect);
    void destroyPipeline(const vk::Pipeline& pipeline);

    bool renderFrame(void (*externalRender)());

    void bindPipeline(const vk::Pipeline& pipeline);
    void bindVertexBuffers(uint32_t count, const vk::Buffer* buffers, const vk::DeviceSize* offsets);
    void pushConstants(const vk::PipelineLayout& layout, const vk::ShaderStageFlags& stage, uint32_t offset, uint32_t size, const void* ptr);
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

    bool reloadSwapchain();
    void destroySwapchain();

    void waitIdle();
    void terminate();
}

#endif//MSCFINALPROJECT_VULKAN_WRAPPER_H
