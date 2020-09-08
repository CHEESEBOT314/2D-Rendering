#ifndef MSCFINALPROJECT_VML_MAT4_HPP
#define MSCFINALPROJECT_VML_MAT4_HPP

#include <vml/vec4.hpp>
#include <vml/mat3.hpp>

namespace vml {
    struct mat4 {
        vec4 cols[4];
        mat4();
        mat4(float c00, float c01, float c02, float c03,
             float c10, float c11, float c12, float c13,
             float c20, float c21, float c22, float c23,
             float c30, float c31, float c32, float c33);
        mat4(const vec4& c0, const vec4& c1, const vec4& c2, const vec4& c3);
        mat4(const mat4& m);

        mat4& operator=(const mat4& m);
        mat4& operator*=(const mat4& m);
        mat4& operator*=(float s);
        mat4& operator/=(float s);

        vec4& operator[](int i);
        vec4 const& operator[](int i) const;

        static mat4 identity();
        static mat4 extend(const mat2& m);
        static mat4 extend(const mat3& m);
    };

    mat4 operator+(const mat4& m);
    mat4 operator-(const mat4& m);

    mat4 operator*(const mat4& m0, const mat4& m1);
    mat4 operator*(const mat4& m, float s);
    mat4 operator*(float s, const mat4& m);
    vec4 operator*(const mat4& m, const vec4& v);
    mat4 operator/(const mat4& m, float s);
}

#endif//MSCFINALPROJECT_VML_MAT4_HPP