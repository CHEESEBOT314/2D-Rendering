#include "vml/vec4.hpp"

namespace vml
{
    vec4::vec4() : x(0), y(0), z(0), w(0) {}
    vec4::vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    vec4::vec4(float x, float y, const vec2& v) : x(x), y(y), z(v.x), w(v.y) {}
    vec4::vec4(float x, const vec2& v, float w) : x(x), y(v.x), z(v.y), w(w) {}
    vec4::vec4(const vec2& v, float z, float w) : x(v.x), y(v.y), z(z), w(w) {}
    vec4::vec4(const vec2& v0, const vec2& v1) : x(v0.x), y(v0.y), z(v1.x), w(v1.y) {}
    vec4::vec4(float x, const vec3& v) : x(x), y(v.x), z(v.y), w(v.z) {}
    vec4::vec4(const vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    vec4::vec4(const vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    vec4& vec4::operator=(const vec4& v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = v.w;
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

    float vec4::magnitude() {
        return sqrt(x*x + y*y + z*z + w*w);
    }

    vec4 operator+(const vec4& v) {
        return vec4(+v.x, +v.y, +v.z, +v.w);
    }
    vec4 operator-(const vec4& v) {
        return vec4(-v.x, -v.y, -v.z, -v.w);
    }

    vec4 operator+(const vec4& v0, const vec4& v1) {
        return vec4(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w);
    }
    vec4 operator-(const vec4& v0, const vec4& v1) {
        return vec4(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w);
    }

    vec4 operator*(const vec4& v, float s) {
        return vec4(v.x * s, v.y * s, v.z * s, v.w * s);
    }
    vec4 operator*(float s, const vec4& v) {
        return vec4(v.x * s, v.y * s, v.z * s, v.w * s);
    }
    vec4 operator/(const vec4& v, float s) {
        return vec4(v.x / s, v.y / s, v.z / s, v.w / s);
    }
};