#version 450
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColour;

void main() {
    outColour = vec4(1.0, 1.0, 1.0, 1.0);
}