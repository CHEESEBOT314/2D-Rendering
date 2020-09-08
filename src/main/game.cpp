#include <game.hpp>

#include <render/render_manager.hpp>

#include <memory>

namespace game {
    namespace {
        struct info {
            uint32_t shader_id;
        };
        std::unique_ptr<info> info_p;
    }
    void init() {
        info_p = std::make_unique<info>();
        if (!render::render_manager::create_graphics_pipeline("default")) {

            return;
        }
        info_p->shader_id = render::render_manager::get_pipeline("default");
    }

    void render() {
        render::render_manager::bind_pipeline(info_p->shader_id);
    }
    void handle_event() {
    }
    void update() {
    }

    bool should_quit() {
        return false;
    }

    void terminate() {
        info_p.reset(nullptr);
    }
}