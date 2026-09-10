#ifndef CRT_VECTOR_HPP
#define CRT_VECTOR_HPP

#include <iostream>
#include <math.h>
class CRT_vector3
{
public:
    CRT_vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    CRT_vector3(float _x, float _y, float _z) noexcept : x(_x), y(_y), z(_z) {};
    
    float length() const noexcept
    {
        return sqrtf(x*x + y*y + z*z);
    }   

    CRT_vector3 getNormalized() const noexcept;

    CRT_vector3& normalize() noexcept;
    
// Operators
public:
    
    CRT_vector3& operator+=(const CRT_vector3& lhs)  noexcept
    {
        x += lhs.x;
        y += lhs.y;
        z += lhs.z;
        return *this;
    }

    CRT_vector3& operator-=(const CRT_vector3& lhs) noexcept
    {
        x -= lhs.x;
        y -= lhs.y;
        z -= lhs.z;
        return *this;
    }

    // multiply this vector by a scalar
    CRT_vector3& operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    CRT_vector3& operator/=(float scalar) noexcept
    {
        float inv = 1.0f / scalar;
        return (*this) *= inv;
    }

    CRT_vector3 operator-() noexcept
    {
        return CRT_vector3(-x,-y,-z);
    }
    
// member fields
public:
    float x, y, z;
};


inline CRT_vector3 operator+ (const CRT_vector3& left, const CRT_vector3& right) noexcept 
{
    return CRT_vector3(left.x + right.x, left.y + right.y, left.z + right.z);
}

inline CRT_vector3 operator- (const CRT_vector3& left, const CRT_vector3& right) noexcept 
{
    return CRT_vector3(left.x - right.x, left.y - right.y, left.z - right.z);
}

// multiply vector by a scalar from the left
inline CRT_vector3 operator*(float scalar, const CRT_vector3& v) noexcept 
{
    return CRT_vector3(scalar * v.x, scalar * v.y, scalar * v.z);
}

// multiply vector by a scalar from the right
inline CRT_vector3 operator*(const CRT_vector3& v, float scalar) noexcept 
{
    return scalar*v;
}

// dot broduct between two vectors
inline float dot(const CRT_vector3& left, const CRT_vector3& right) noexcept
{
    return (left.x * right.x) + (left.y * right.y) +(left.z * right.z);
}

// operator for dot broduct between two vectors
inline float operator*(const CRT_vector3& left, const CRT_vector3& right) noexcept
{
    return dot(left,right);
}

// divide a vector by a scalar
inline CRT_vector3 operator/(const CRT_vector3& v, float scalar) noexcept
{
    float inv = 1.0f / scalar;
    return v * inv;
}

// cross product between two vectors
inline CRT_vector3 cross(const CRT_vector3& left, const CRT_vector3& right) noexcept
{
    return CRT_vector3
    (
        left.y*right.z - left.z*right.y,
        left.z*right.x - left.x*right.z,
        left.x*right.y - left.y*right.x 
    );
}

// operator for cross product between two vectors
inline CRT_vector3 operator^(const CRT_vector3& left, const CRT_vector3& right) noexcept
{
    return cross(left,right);
}

inline CRT_vector3 CRT_vector3::getNormalized() const noexcept
{
    return (*this) / length();
}

inline CRT_vector3& CRT_vector3::normalize() noexcept
{
    *this = *this / length();
    return *this;
}

inline std::ostream& operator<<(std::ostream& os, const CRT_vector3& v)
{
    os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    return os;
}

#endif