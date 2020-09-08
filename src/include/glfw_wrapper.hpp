#ifndef MSCFINALPROJECT_GLFWWRAPPER_HPP
#define MSCFINALPROJECT_GLFWWRAPPER_HPP

#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

namespace glfw_wrapper {
    std::vector<const char*> init();
    bool create_surface(const vk::Instance& instance, vk::SurfaceKHR& surface_khr);

    void get_resolution(int* width, int* height);

    void poll_events();

    bool should_quit();
    void terminate();
}

#endif//MSCFINALPROJECT_GLFWWRAPPER_HPP
