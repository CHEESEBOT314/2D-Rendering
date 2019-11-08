#include "vml/vec4.hpp"

namespace vml
{
    vec4::vec4() : data{0.0f, 0.0f, 0.0f, 0.0f} {}
    vec4::vec4(float x, float y, float z, float w) : data{x, y, z, w} {}
    vec4::vec4(float x, float y, const vec2& v) : data{x, y, v[0], v[1]} {}
    vec4::vec4(float x, const vec2& v, float w) : data{x, v[0], v[1], w} {}
    vec4::vec4(const vec2& v, float z, float w) : data{v[0], v[1], z, w} {}
    vec4::vec4(const vec2& v0, const vec2& v1) : data{v0[0], v0[1], v1[0], v1[1]} {}
    vec4::vec4(float x, const vec3& v) : data{x, v[0], v[1], v[2]} {}
    vec4::vec4(const vec3& v, float w) : data{v[0], v[1], v[2], w} {}
    vec4::vec4(const vec4& v) : data{v[0], v[1], v[2], v[3]} {}

    vec4& vec4::operator=(const vec4& v) {
        this->data[0] = v[0];
        this->data[1] = v[1];
        this->data[2] = v[2];
        this->data[3] = v[3];
        return *this;
    }
    vec4& vec4::operator+=(const vec4& v) {
        return (*this = (*this + v));
    }
    vec4& vec4::operator-=(const vec4& v) {
        return (*this = (*this - v));
    }
    vec4& vec4::operator*=(float s) {
        return (*this = (*this * s));
    }
    vec4& vec4::operator/=(float s) {
        return (*this = (*this / s));
    }

    float& vec4::operator[](int i) {
        return this->data[i];
    }
    float const& vec4::operator[](int i) const {
        return this->data[i];
    }

    float vec4::magnitude() const {
        return std::sqrt(this->data[0]*this->data[0] + this->data[1]*this->data[1] + this->data[2]*this->data[2] + this->data[3]*this->data[3]);
    }

    vec4 operator+(const vec4& v) {
        return vec4(+v[0], +v[1], +v[2], +v[3]);
    }
    vec4 operator-(const vec4& v) {
        return vec4(-v[0], -v[1], -v[2], -v[3]);
    }

    vec4 operator+(const vec4& v0, const vec4& v1) {
        return vec4(v0[0] + v1[0], v0[1] + v1[1], v0[2] + v1[2], v0[3] + v1[3]);
    }
    vec4 operator-(const vec4& v0, const vec4& v1) {
        return vec4(v0[0] - v1[0], v0[1] - v1[1], v0[2] - v1[2], v0[3] - v1[3]);
    }

    vec4 operator*(const vec4& v, float s) {
        return vec4(v[0] * s, v[1] * s, v[2] * s, v[3] * s);
    }
    vec4 operator*(float s, const vec4& v) {
        return vec4(v[0] * s, v[1] * s, v[2] * s, v[3] * s);
    }
    vec4 operator/(const vec4& v, float s) {
        return vec4(v[0] / s, v[1] / s, v[2] / s, v[3] / s);
    }
};