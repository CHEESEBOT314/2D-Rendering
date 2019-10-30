cmake_minimum_required(VERSION 3.14)
project(MScFinalProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

                ### GLFW SETUP ###
###============================================###
set(GLFW_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS      OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL          OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
###============================================###

set(ENV{VULKAN_SDK} "/Users/eddie/vulkansdk-macos-1.1.121.1/macOS")

find_package(Vulkan REQUIRED)

if (DEBUG)
    add_definitions(-DDEBUG_MODE)
endif()

set(SOURCES src/main/Main.cpp

            src/main/GlfwWrapper.cpp
            src/main/Game.cpp
            src/main/VulkanWrapper.cpp

            src/main/render/RenderManager.cpp

            src/main/platform/Windows.cpp)

file(GLOB_RECURSE SHADERS shaders/*.glsl)

set(APP_NAME "2D")

add_executable(${APP_NAME} ${SOURCES})

target_link_libraries(${APP_NAME} glfw Vulkan::Vulkan)
target_include_directories(${APP_NAME} PRIVATE src/include glfw/include Vulkan::Vulkan)