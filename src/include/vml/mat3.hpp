#ifndef MSCFINALPROJECT_VML_MAT3_HPP
#define MSCFINALPROJECT_VML_MAT3_HPP

#include "vml/vec3.hpp"
#include "vml/mat2.hpp"

namespace vml {
    struct mat3 {
        vec3 cols[3];
        mat3();
        mat3(float c00, float c01, float c02,
             float c10, float c11, float c12,
             float c20, float c21, float c22);
        mat3(const vec3& c0, const vec3& c1, const vec3& c2);
        mat3(const mat3& m);

        mat3& operator=(const mat3& m);
        mat3& operator*=(const mat3& m);
        mat3& operator*=(float s);
        mat3& operator/=(float s);

        vec3& operator[](int i);
        vec3 const& operator[](int i) const;

        static mat3 identity();
        static mat3 extend(const mat2& m);
    };

    mat3 operator+(const mat3& m);
    mat3 operator-(const mat3& m);

    mat3 operator*(const mat3& m0, const mat3& m1);
    mat3 operator*(const mat3& m, float s);
    mat3 operator*(float s, const mat3& m);
    vec3 operator*(const mat3& m, const vec3& v);
    mat3 operator/(const mat3& m, float s);
}

#endif//MSCFINALPROJECT_VML_MAT3_HPP