#version 450
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform Info {
    mat4 p;
    mat4 v;
    mat4 m;
    mat3 textureTransform;
} info;

layout(location = 0) in vec2 posIn;
layout(location = 1) in vec2 uvIn;

layout(location = 0) out vec2 uvOut;
layout(location = 1) out vec3 normalOut;

void main() {
    uvOut = (info.textureTransform * vec3(uvIn, 1.0)).xy;

    mat4 mv = info.v * info.m;
    normalOut = mat3(mv) * vec3(0.0, 0.0, 1.0);
    gl_Position = info.p * mv * vec4(posIn, 0.0, 1.0);
}