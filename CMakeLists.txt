cmake_minimum_required(VERSION 3.14)
project(MScFinalProject)
set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")

                ### GLFW SETUP ###
###============================================###
set(GLFW_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS      OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL          OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
###============================================###

find_package(Vulkan REQUIRED)

if (DEBUG)
    add_definitions(-DDEBUG_MODE)
endif()

set(SOURCES src/main/main.cpp

        src/main/glfw_wrapper.cpp
        src/main/vulkan_wrapper.cpp)

file(GLOB_RECURSE SHADERS shaders/*.glsl)

set(APP_NAME "2D")

add_executable(${APP_NAME} ${SOURCES})

target_link_libraries(${APP_NAME} glfw Vulkan::Vulkan)
target_include_directories(${APP_NAME} PRIVATE src/include glfw/include Vulkan::Vulkan)