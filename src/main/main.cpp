#include "game.hpp"
#include "glfw_wrapper.hpp"
#include "vulkan_wrapper.hpp"
#include "platform/platform.hpp"
#include "render/render_manager.hpp"
#include "render/sprite_manager.hpp"
#include "resource/resource_manager.hpp"

int main(int argc, char** args) {
    std::vector<const char*> extensions = glfw_wrapper::init();
    if (extensions.empty()) {
        return 0;
    }
    if (!vulkan_wrapper::create_instance(extensions) ||
        !vulkan_wrapper::create_surface(glfw_wrapper::create_surface, glfw_wrapper::get_resolution) ||
        !vulkan_wrapper::create_others()) {
        return 0;
    }
    resource::resource_manager::init(platform::files::get_resource_folder(), platform::files::FILE_SEPARATOR);

    render::render_manager::init();
    render::render_manager::load_shaders();

    render::sprite_manager::init();

    game::init();

    while (!(glfw_wrapper::should_quit() || game::should_quit())) {
        glfw_wrapper::poll_events();
        game::update();
        if (!vulkan_wrapper::render_frame(game::render)) {
            break;
        }
    }
    vulkan_wrapper::wait_idle();

    render::render_manager::terminate();
    vulkan_wrapper::terminate();
    glfw_wrapper::terminate();
    return 0;
}

