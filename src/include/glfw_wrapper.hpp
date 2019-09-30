#ifndef MSCFINALPROJECT_GLFW_WRAPPER_HPP
#define MSCFINALPROJECT_GLFW_WRAPPER_HPP

#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

namespace glfw_wrapper {
    const char** init(uint32_t* ext_count);
    bool createSurface(vk::Instance instance, vk::SurfaceKHR* surfaceKhr);

    void terminate();
}

#endif //MSCFINALPROJECT_GLFW_WRAPPER_HPP
