#ifndef MSCFINALPROJECT_VML_QUATERNION_HPP
#define MSCFINALPROJECT_VML_QUATERNION_HPP

#include "vml/vec4.hpp"

namespace vml {
    struct quaternion {
        float data[4];

        quaternion();
        quaternion(float x, float y, float z, float w);
        quaternion(const quaternion& q);

        quaternion& operator=(const quaternion& q);
        quaternion& operator*=(const quaternion& q);
        quaternion& operator*=(float s);
        quaternion& operator/=(float s);

        float& operator[](int i);
        float const& operator[](int i) const;

        static quaternion identity();
        quaternion inverse();
        vec4 rotate(const vec4& v);
    };

    quaternion operator+(const quaternion& q);
    quaternion operator-(const quaternion& q);

    quaternion operator*(const quaternion& q1, const quaternion& q2);
    quaternion operator*(const quaternion& q, float s);
    quaternion operator*(float s, const quaternion& q);
    quaternion operator/(const quaternion& q, float s);
}

#endif//MSCFINALPROJECT_VML_QUATERNION_HPP