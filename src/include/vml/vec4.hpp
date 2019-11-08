#ifndef MSCFINALPROJECT_VML_VEC4_HPP
#define MSCFINALPROJECT_VML_VEC4_HPP

#include "vml/vec3.hpp"

namespace vml {
    struct vec4 {
        float data[4];
        vec4();
        vec4(float x, float y, float z, float w);
        vec4(float x, float y, const vec2& v);
        vec4(float x, const vec2& v, float w);
        vec4(const vec2& v, float z, float w);
        vec4(const vec2& v0, const vec2& v1);
        vec4(float x, const vec3& v);
        vec4(const vec3& v, float w);
        vec4(const vec4& v);

        vec4& operator=(const vec4& v);
        vec4& operator+=(const vec4& v);
        vec4& operator-=(const vec4& v);
        vec4& operator*=(float s);
        vec4& operator/=(float s);

        float& operator[](int i);
        float const& operator[](int i) const;

        float magnitude() const;
    };

    vec4 operator+(const vec4& v);
    vec4 operator-(const vec4& v);

    vec4 operator+(const vec4& v0, const vec4& v1);
    vec4 operator-(const vec4& v0, const vec4& v1);

    vec4 operator*(const vec4& v, float s);
    vec4 operator*(float s, const vec4& v);
    vec4 operator/(const vec4& v, float s);
}

#endif//MSCFINALPROJECT_VML_VEC4_HPP