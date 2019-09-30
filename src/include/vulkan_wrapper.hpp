#ifndef MSCFINALPROJECT_VULKAN_WRAPPER_H
#define MSCFINALPROJECT_VULKAN_WRAPPER_H

#include "vulkan/vulkan.hpp"

namespace vulkan_wrapper {

    bool createInstance(const char** ext_list, uint32_t ext_count);
    bool createSurface(bool(*fn)(vk::Instance, vk::SurfaceKHR*));

    void terminate();
}

#endif //MSCFINALPROJECT_VULKAN_WRAPPER_H
