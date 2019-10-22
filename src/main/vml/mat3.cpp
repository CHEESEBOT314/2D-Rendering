#include "vml/mat3.hpp"

namespace vml {
    mat3::mat3() : c0(), c1(), c2() {}
    mat3::mat3(float c00, float c01, float c02,
               float c10, float c11, float c12,
               float c20, float c21, float c22)
                : c0(c00, c01, c02), c1(c10, c11, c12), c2(c20, c21, c22) {}
    mat3::mat3(const vec3& c0, const vec3& c1, const vec3& c2) : c0(c0), c1(c1), c2(c2) {}
    mat3::mat3(const mat3& m) : c0(m.c0), c1(m.c1), c2(m.c2) {}

    mat3& mat3::operator=(const mat3& m) {
        this->c0 = m.c0;
        this->c1 = m.c1;
        this->c2 = m.c2;
        return *this;
    }
    mat3& mat3::operator*=(const mat3& m) {
        return (*this = (*this * m));
    }
    mat3& mat3::operator*=(float s) {
        return (*this = (*this * s));
    }
    mat3& mat3::operator/=(float s) {
        return (*this = (*this / s));
    }

    vec3& mat3::operator[](int i) {
        return this->cols[i];
    }
    vec3 const& mat3::operator[](int i) const {
        return this->cols[i];
    }

    static mat3 mat3::identity() {
        return mat3(
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f);
    }
    static mat3 mat3::extend(const mat2& m) {
        return mat3(
            m.c0.x, m.c0.y, 0.0f,
            m.c1.x, m.c1.y, 0.0f,
            0.0f,   0.0f,   1.0f);
    }

    mat3 operator+(const mat3& m) {
        return mat3(+m.c0, +m.c1, +m.c2);
    }
    mat3 operator-(const mat3& m) {
        return mat3(-m.c0, -m.c1, -m.c2);
    }

    mat3 operator*(const mat3& m0, const mat3& m1) {
        return mat3(
            m0[0][0]*m1[0][0] + m0[1][0]*m1[0][1] + m0[2][0]*m1[0][2],
            m0[0][1]*m1[0][0] + m0[1][1]*m1[0][1] + m0[2][1]*m1[0][2],
            m0[0][2]*m1[0][0] + m0[1][2]*m1[0][1] + m0[2][2]*m1[0][2],

            m0[0][0]*m1[1][0] + m0[1][0]*m1[1][1] + m0[2][0]*m1[1][2],
            m0[0][1]*m1[1][0] + m0[1][1]*m1[1][1] + m0[2][1]*m1[1][2],
            m0[0][2]*m1[1][0] + m0[1][2]*m1[1][1] + m0[2][2]*m1[1][2],

            m0[0][0]*m1[2][0] + m0[1][0]*m1[2][1] + m0[2][0]*m1[2][2],
            m0[0][1]*m1[2][0] + m0[1][1]*m1[2][1] + m0[2][1]*m1[2][2],
            m0[0][2]*m1[2][0] + m0[1][2]*m1[2][1] + m0[2][2]*m1[2][2]);
    }
    mat3 operator*(const mat3& m, float s) {
        return mat3(m.c0 * s, m.c1 * s, m.c2 * s);
    }
    mat3 operator*(float s, const mat3& m) {
        return mat3(m.c0 * s, m.c1 * s, m.c2 * s);
    }
    vec3 operator*(const mat3& m, const vec3& v) {
        return vec3(
            m[0][0]*v[0] + m0[1][0]*v[1] + m0[2][0]*v[2],
            m[0][1]*v[0] + m0[1][1]*v[1] + m0[2][1]*v[2],
            m[0][2]*v[0] + m0[1][2]*v[1] + m0[2][2]*v[2]);
    }
    mat3 operator/(const mat3& m, float s) {
        return mat3(m.c0 / s, m.c1 / s, m.c2 / s);
    }
}