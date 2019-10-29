#ifndef MSCFINALPROJECT_GLFWWRAPPER_HPP
#define MSCFINALPROJECT_GLFWWRAPPER_HPP

#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

namespace GlfwWrapper {
    std::vector<const char*> init();
    bool createSurface(vk::Instance instance, vk::SurfaceKHR* surfaceKhr);

    void getResolution(int* width, int* height);

    void pollEvents();

    bool shouldQuit();
    void terminate();
}

#endif//MSCFINALPROJECT_GLFWWRAPPER_HPP
