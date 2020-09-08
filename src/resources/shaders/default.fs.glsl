#version 450
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform Info {
    mat4 p;
    mat4 v;
    mat4 m;
    mat3 textureTransform;
    mat4 colourMult;
} info;

layout(location = 0) in vec2 uvIn;
layout(location = 1) in vec3 normalIn;

layout(location = 0) out vec4 outColour;

void main() {
    outColour = vec4(1.0, 1.0, 1.0, 1.0);
}