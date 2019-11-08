#version 450
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

void main() {
    gl_Position = vec4(pos, 0.0, 1.0);
}