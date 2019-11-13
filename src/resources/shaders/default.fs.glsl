#version 450
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 uvIn;
layout(location = 1) in vec3 normalIn;

layout(location = 0) out vec4 outColour;

void main() {
    outColour = vec4(1.0, 1.0, 1.0, 1.0);
}