#include "render/render_manager.hpp"

#include "vulkan_wrapper.hpp"
#include "render/push_constants.hpp"
#include "render/vertex.hpp"
#include "resource/resource_manager.hpp"

#include <map>

namespace render::render_manager {
        namespace {
            struct pipeline {
                vk::PipelineLayout layout;
                vk::Pipeline pl;
            };

            struct info {
                std::map<std::string, uint32_t> name_id_map;
                std::map<uint32_t, pipeline> id_pipeline_map;
                uint32_t next_id = 1;
                bool loaded = false;

                vk::Buffer rect_2D;
                vk::DeviceMemory rect_2D_memory;
                vk::DeviceSize* offsets = nullptr;

                push_constants current_pc;
                pipeline* current_pl = nullptr;
            };
            std::unique_ptr<info> info_p;

            bool load_pipeline(const std::string& name, pipeline& pipeline) {
                vk::ShaderModule vert, frag;
                if (!vulkan_wrapper::create_shader_module(vert,
                                                       resource::resource_manager::read_binary_file(name + ".vs.spv",
                                                                                                   {"shaders"})) ||
                    !vulkan_wrapper::create_shader_module(frag,
                                                       resource::resource_manager::read_binary_file(name + ".fs.spv",
                                                                                                   {"shaders"}))) {
                    vulkan_wrapper::destroy_shader_module(vert);
                    vulkan_wrapper::destroy_shader_module(frag);
                    return false;
                }
                vk::PipelineShaderStageCreateInfo shader_stage_create_infos[2];
                shader_stage_create_infos[0] = vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                                                              vk::ShaderStageFlagBits::eVertex, vert,
                                                                              "main");
                shader_stage_create_infos[1] = vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                                                              vk::ShaderStageFlagBits::eFragment, frag,
                                                                              "main");

                vk::PushConstantRange push_constant_range = {vk::ShaderStageFlagBits::eVertex,
                                                           0,
                                                           sizeof(push_constants)};

                vk::PipelineLayoutCreateInfo pipeline_layout_create_info = {vk::PipelineLayoutCreateFlags(),
                                                                         0,
                                                                         nullptr,
                                                                         1,
                                                                         &push_constant_range};
                if (!vulkan_wrapper::create_pipeline_layout(pipeline.layout, pipeline_layout_create_info)) {
                    vulkan_wrapper::destroy_shader_module(vert);
                    vulkan_wrapper::destroy_shader_module(frag);
                    return false;
                }
                vk::VertexInputBindingDescription vertexInputBindingDescription = {0,
                                                                                   sizeof(vertex),
                                                                                   vk::VertexInputRate::eVertex};
                vk::VertexInputAttributeDescription vertexInputAttributeDescriptions[2];
                vertexInputAttributeDescriptions[0] = {0, 0, vk::Format::eR32G32Sfloat, (uint32_t)offsetof(vertex, pos)};
                vertexInputAttributeDescriptions[1] = {1, 0, vk::Format::eR32G32Sfloat, (uint32_t)offsetof(vertex, uv)};

                if (!vulkan_wrapper::create_pipeline(pipeline.pl, pipeline.layout, 2, shader_stage_create_infos, 1, &vertexInputBindingDescription, 2, vertexInputAttributeDescriptions, 1.0f)) {
                    vulkan_wrapper::destroy_shader_module(vert);
                    vulkan_wrapper::destroy_shader_module(frag);
                    vulkan_wrapper::destroy_pipeline_layout(pipeline.layout);
                    return false;
                }

                vulkan_wrapper::destroy_shader_module(vert);
                vulkan_wrapper::destroy_shader_module(frag);
                return true;
            }
        }

        void init() {
            info_p = std::make_unique<info>();
            static const std::vector<vertex> vertices_2D = {
                    {{0.0f, 0.0f}, {0.0f, 0.0f}},
                    {{1.0f, 0.0f}, {1.0f, 0.0f}},
                    {{1.0f, 1.0f}, {1.0f, 1.0f}},
                    {{0.0f, 0.0f}, {0.0f, 0.0f}},
                    {{1.0f, 1.0f}, {1.0f, 1.0f}},
                    {{0.0f, 1.0f}, {0.0f, 1.0f}}};
            vulkan_wrapper::create_vertex_buffer(info_p->rect_2D, info_p->rect_2D_memory, sizeof(vertex) * vertices_2D.size());
            vulkan_wrapper::map_vertex_buffer(info_p->rect_2D_memory, sizeof(vertex) * vertices_2D.size(), vertices_2D.data());
            info_p->offsets = new vk::DeviceSize[1]{0};
            reset_push_constants();
        }

        bool create_graphics_pipeline(const std::string& name) {
            info_p->name_id_map.insert(std::pair<const std::string, uint32_t>(name, info_p->next_id));
            if (info_p->loaded) {
                pipeline pl;
                if (!load_pipeline(name, pl)) {
                    return false;
                }
                info_p->id_pipeline_map.insert(std::pair<const uint32_t, pipeline>(info_p->next_id, pl));
            }
            info_p->next_id++;
            return true;
        }

        uint32_t get_pipeline(const std::string& name) {
            auto it = info_p->name_id_map.find(name);
            if (it != info_p->name_id_map.end()) {
                return it->second;
            }
            return 0;
        }

        void bind_pipeline(uint32_t id) {
            if (id > 0) {
                auto it = info_p->id_pipeline_map.find(id);
                if (it != info_p->id_pipeline_map.end()) {
                    vulkan_wrapper::bind_pipeline(it->second.pl);
                    info_p->current_pl = &it->second;
                }
            }
        }

        void reset_push_constants() {
            info_p->current_pc.p = vml::mat4::identity();
            info_p->current_pc.v = vml::mat4::identity();
            info_p->current_pc.m = vml::mat4::identity();
            info_p->current_pc.tt = vml::mat3(1.0f, 0.0f, 0.0f,
                                             0.0f, 1.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f);
            info_p->current_pc.cm = vml::mat4::identity();
        }
        void set_perspective(const vml::mat4& pers) {
            info_p->current_pc.p = pers;
        }
        void set_view(const vml::mat4& view) {
            info_p->current_pc.v = view;
        }
        void set_model(const vml::mat4& mode) {
            info_p->current_pc.m = mode;
        }
        void set_texture_transform(const vml::mat3& tt) {
            info_p->current_pc.tt = tt;
        }
        void set_colour_mult(const vml::mat4& cm) {
            info_p->current_pc.cm = cm;
        }

        void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
            vulkan_wrapper::push_constants(info_p->current_pl->layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(push_constants), &info_p->current_pc);
            vulkan_wrapper::draw(vertex_count, instance_count, first_vertex, first_instance);
        }
        void draw_rect_2D() {
            vulkan_wrapper::bind_vertex_buffers(1, &info_p->rect_2D, info_p->offsets);
            draw(6, 1, 0, 0);
        }

        bool load_shaders() {
            for (const std::pair<const std::string, uint32_t>& nPair : info_p->name_id_map) {
                pipeline pl;
                if (!load_pipeline(nPair.first, pl)) {
                    unload_shaders();
                    return false;
                }
                info_p->id_pipeline_map.insert(std::pair<const uint32_t, pipeline>(nPair.second, pl));
            }
            return (info_p->loaded = true);
        }

        void unload_shaders() {
            for (const std::pair<const uint32_t, pipeline>& pPair : info_p->id_pipeline_map) {
                vulkan_wrapper::destroy_pipeline_layout(pPair.second.layout);
                vulkan_wrapper::destroy_pipeline(pPair.second.pl);
            }
            info_p->id_pipeline_map.clear();
            info_p->loaded = false;
        }

        bool reload_shaders() {
            unload_shaders();
            return load_shaders();
        }

        void terminate() {
            if (info_p->loaded) {
                unload_shaders();
            }
            delete[] info_p->offsets;
            vulkan_wrapper::destroy_vertex_buffer(info_p->rect_2D, info_p->rect_2D_memory);
            info_p->name_id_map.clear();
            info_p.reset(nullptr);
        }
    }