#include "vml/mat2.hpp"

namespace vml {
    mat2::mat2() : cols{vec2(), vec2()} {};
    mat2::mat2(float c00, float c01,
               float c10, float c11)
                : cols{vec2(c00, c01), vec2(c10, c11)} {}
    mat2::mat2(const vec2& c0, const vec2& c1) : cols{c0, c1} {}
    mat2::mat2(const mat2& m) : cols{m[0], m[1]} {}

    mat2& mat2::operator=(const mat2& m) {
        this->cols[0] = m[0];
        this->cols[1] = m[1];
        return *this;
    }
    mat2& mat2::operator*=(const mat2& m) {
        return (*this = (*this * m));

    }
    mat2& mat2::operator*=(float s) {
        return (*this = (*this * s));
    }
    mat2& mat2::operator/=(float s) {
        return (*this = (*this / s));
    }

    vec2& mat2::operator[](int i) {
        return this->cols[i];
    }
    vec2 const& mat2::operator[](int i) const {
        return this->cols[i];
    }

    mat2 mat2::identity() {
        return mat2(1.0f, 0.0f, 0.0f, 1.0f);
    }

    mat2 operator+(const mat2& m) {
        return mat2(+m[0], +m[1]);
    }
    mat2 operator-(const mat2& m) {
        return mat2(-m[0], -m[1]);
    }

    mat2 operator*(const mat2& m0, const mat2& m1) {
        return mat2(
            m0[0][0]*m1[0][0] + m0[1][0]*m1[0][1], m0[0][1]*m1[0][0] + m0[1][1]*m1[0][1],
            m0[0][0]*m1[1][0] + m0[1][0]*m1[1][1], m0[0][1]*m1[1][0] + m0[1][1]*m1[1][1]);
    }
    mat2 operator*(const mat2& m, float s) {
        return mat2(m[0] * s, m[1] * s);
    }
    mat2 operator*(float s, const mat2& m) {
        return mat2(m[0] * s, m[1] * s);
    }
    vec2 operator*(const mat2& m, const vec2& v) {
        return vec2(
            m[0][0]*v[0] + m[1][0]*v[1],
            m[0][1]*v[0] + m[1][1]*v[1]);
    }
    mat2 operator/(const mat2& m, float s) {
        return mat2(m[0] / s, m[1] / s);
    }
}