#ifndef MSCFINALPROJECT_VML_MAT2_HPP
#define MSCFINALPROJECT_VML_MAT2_HPP

#include <vml/vec2.hpp>

namespace vml {
    struct mat2 {
        union {
            vec2 cols[2];
            struct { vec2 c0, c1; };
        };
        mat2();
        mat2(float c00, float c01,
             float c10, float c11);
        mat2(const vec2& c0, const vec2& c1);
        mat2(const mat2& m);

        mat2& operator=(const mat2& m);
        mat2& operator*=(const mat2& m);
        mat2& operator*=(float s);
        mat2& operator/=(float s);

        vec2& operator[](int i);
        vec2 const& operator[](int i) const;

        static mat2 identity();
    };

    mat2 operator+(const mat2& m);
    mat2 operator-(const mat2& m);

    mat2 operator*(const mat2& m0, const mat2& m1);
    mat2 operator*(const mat2& m, float s);
    mat2 operator*(float s, const mat2& m);
    vec2 operator*(const mat2& m, const vec2& v);
    mat2 operator/(const mat2& m, float s);
}

#endif//MSCFINALPROJECT_VML_MAT2_HPP