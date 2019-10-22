#include "vml/vec3.hpp"

namespace vml
{
    vec3::vec3() : x(0), y(0), z(0) {}
    vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    vec3::vec3(float x, const vec2& v) : x(x), y(v.x), z(v.y) {}
    vec3::vec3(const vec2& v, float z) : x(v.x), y(v.y), z(z) {}
    vec3::vec3(const vec3& v) : x(v.x), y(v.y), z(v.z) {}

    vec3& vec3::operator=(const vec3& v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        return *this;
    }
    vec3& vec3::operator+=(const vec3& v) {
        return (*this = (*this + v));
    }
    vec3& vec3::operator-=(const vec3& v) {
        return (*this = (*this - v));
    }
    vec3& vec3::operator*=(float s) {
        return (*this = (*this * s));
    }
    vec3& vec3::operator/=(float s) {
        return (*this = (*this / s));
    }

    float& vec3::operator[](int i) {
        return this->data[i];
    }
    float const& vec3::operator[](int i) const {
        return this->data[i];
    }

    float vec3::magnitude() {
        return sqrt(x*x + y*y + z*z);
    }

    vec3 operator+(const vec3& v) {
        return vec3(+v.x, +v.y, +v.z);
    }
    vec3 operator-(const vec3& v) {
        return vec3(-v.x, -v.y, -v.z);
    }

    vec3 operator+(const vec3& v0, const vec3& v1) {
        return vec3(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
    }
    vec3 operator-(const vec3& v0, const vec3& v1) {
        return vec3(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
    }

    vec3 operator*(const vec3& v, float s) {
        return vec3(v.x * s, v.y * s, v.z * s);
    }
    vec3 operator*(float s, const vec3& v) {
        return vec3(v.x * s, v.y * s, v.z * s);
    }
    vec3 operator/(const vec3& v, float s) {
        return vec3(v.x / s, v.y / s, v.z / s);
    }
};