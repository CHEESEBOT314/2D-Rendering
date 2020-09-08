#ifndef MSCFINALPROJECT_VML_VEC3_HPP
#define MSCFINALPROJECT_VML_VEC3_HPP

#include <vml/vec2.hpp>

namespace vml {
    struct vec3 {
        float data[3];
        vec3();
        vec3(float x, float y, float z);
        vec3(float x, const vec2& v);
        vec3(const vec2& v, float z);
        vec3(const vec3& v);

        vec3& operator=(const vec3& v);
        vec3& operator+=(const vec3& v);
        vec3& operator-=(const vec3& v);
        vec3& operator*=(float s);
        vec3& operator/=(float s);

        float& operator[](int i);
        float const& operator[](int i) const;

        float magnitude() const;
    };

    vec3 operator+(const vec3& v);
    vec3 operator-(const vec3& v);

    vec3 operator+(const vec3& v0, const vec3& v1);
    vec3 operator-(const vec3& v0, const vec3& v1);

    vec3 operator*(const vec3& v, float s);
    vec3 operator*(float s, const vec3& v);
    vec3 operator/(const vec3& v, float s);
}

#endif//MSCFINALPROJECT_VML_VEC3_HPP