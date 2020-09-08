#ifndef MSCFINALPROJECT_RENDER_RENDERMANAGER_HPP
#define MSCFINALPROJECT_RENDER_RENDERMANAGER_HPP

#include <string>
#include <vml/mat4.hpp>

namespace render::render_manager {
    void init();

    bool create_graphics_pipeline(const std::string& name);
    uint32_t get_pipeline(const std::string& name);

    void bind_pipeline(uint32_t id);

    void reset_push_constants();
    void set_perspective(const vml::mat4& pers);
    void set_view(const vml::mat4& view);
    void set_model(const vml::mat4& mode);
    void set_texture_transform(const vml::mat3& tt);
    void set_colour_mult(const vml::mat4& cm);

    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);
    void draw_rect_2D();

    bool load_shaders();
    void unload_shaders();
    bool reload_shaders();


    void terminate();
}

#endif//MSCFINALPROJECT_RENDER_RENDERMANAGER_HPP
