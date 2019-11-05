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

set(APP_NAME "2D")

if (DEBUG)
    add_definitions(-DDEBUG_MODE)
endif()

set(SOURCES src/main/Main.cpp

        src/main/GlfwWrapper.cpp
        src/main/Game.cpp
        src/main/VulkanWrapper.cpp

        src/main/render/RenderManager.cpp

        src/main/resource/ResourceManager.cpp

        src/main/vml/mat2.cpp
        src/main/vml/mat3.cpp
        src/main/vml/mat4.cpp
        src/main/vml/quaternion.cpp
        src/main/vml/transform.cpp
        src/main/vml/vec2.cpp
        src/main/vml/vec3.cpp
        src/main/vml/vec4.cpp)

if (WIN32)
    set(PLATFORM_SOURCES src/main/platform/Windows.cpp)
    set(RESOURCE_DIR ${PROJECT_SOURCE_DIR}/bin/resources)

    add_executable(${APP_NAME} ${SOURCES} ${PLATFORM_SOURCES})
endif()
if (APPLE)
    find_library(CORE_FOUNDATION CoreFoundation)

    set(PLATFORM_SOURCES src/main/platform/Mac.cpp)
    set(RESOURCE_DIR ${PROJECT_SOURCE_DIR}/bin/${APP_NAME}.app/Contents/Resources)

    add_executable(${APP_NAME} MACOSX_BUNDLE ${SOURCES} ${PLATFORM_SOURCES})
    target_link_libraries(${APP_NAME} ${CORE_FOUNDATION})
endif()

add_custom_command(TARGET ${APP_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/resources ${RESOURCE_DIR})

target_link_libraries(${APP_NAME} glfw Vulkan::Vulkan)
target_include_directories(${APP_NAME} PRIVATE src/include glfw/include Vulkan::Vulkan)

