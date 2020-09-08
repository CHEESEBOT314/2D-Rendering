#ifndef MSCFINALPROJECT_VULKAN_WRAPPER_H
#define MSCFINALPROJECT_VULKAN_WRAPPER_H

#include "vulkan/vulkan.hpp"

namespace vulkan_wrapper {

    bool create_instance(std::vector<const char*> extensions);
    bool create_surface(bool(*fn)(const vk::Instance&, vk::SurfaceKHR&), void (*r)(int*, int*));
    bool create_others();
    bool create_swapchain();

    bool create_vertex_buffer(vk::Buffer& buffer, vk::DeviceMemory& memory, uint32_t size);
    void map_vertex_buffer(const vk::DeviceMemory& memory, uint32_t size, const void* data);
    void destroy_vertex_buffer(const vk::Buffer& buffer, const vk::DeviceMemory& memory);

    bool create_shader_module(vk::ShaderModule& shader_module, const std::vector<uint8_t>& src);
    void destroy_shader_module(const vk::ShaderModule& shader_module);

    bool create_pipeline_layout(vk::PipelineLayout& pipeline_layout, const vk::PipelineLayoutCreateInfo& pipeline_layout_create_info);
    void destroy_pipeline_layout(const vk::PipelineLayout& pipeline_layout);
    bool create_pipeline(vk::Pipeline& pipeline, const vk::PipelineLayout& pipeline_layout, uint32_t shader_module_count, const vk::PipelineShaderStageCreateInfo* shader_modules, uint32_t vertex_binding_description_count, const vk::VertexInputBindingDescription* vertex_binding_descriptions, uint32_t vertex_attribute_description_count, const vk::VertexInputAttributeDescription* vertex_attribute_descriptions, float target_aspect);
    void destroy_pipeline(const vk::Pipeline& pipeline);

    bool render_frame(void (*external_render)());

    void bind_pipeline(const vk::Pipeline& pipeline);
    void bind_vertex_buffers(uint32_t count, const vk::Buffer* buffers, const vk::DeviceSize* offsets);
    void push_constants(const vk::PipelineLayout& layout, const vk::ShaderStageFlags& stage, uint32_t offset, uint32_t size, const void* ptr);
    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);

    bool reload_swapchain();
    void destroy_swapchain();

    void wait_idle();
    void terminate();
}

#endif//MSCFINALPROJECT_VULKAN_WRAPPER_H
