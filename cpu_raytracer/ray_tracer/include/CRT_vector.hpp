#pragma once

#include <math.h>
class CRT_vector
{
public:
    CRT_vector() noexcept : x(0), y(0), z(0) {}
    CRT_vector(float x, float y, float z) noexcept : x(x), y(y), z(z) {};
    
    float length() const noexcept
    {
        return sqrtf(x*x + y*y + z*z);
    }   

    CRT_vector getNormalized() const noexcept;

    CRT_vector& normalize() noexcept;
    
// Operators
public:
    
    CRT_vector& operator+=(const CRT_vector& lhs)  noexcept
    {
        x += lhs.x;
        y += lhs.y;
        z += lhs.z;
        return *this;
    }

    CRT_vector& operator-=(const CRT_vector& lhs) noexcept
    {
        x -= lhs.x;
        y -= lhs.y;
        z -= lhs.z;
        return *this;
    }

    // multiply this vector by a scalar
    CRT_vector& operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    CRT_vector& operator/=(float scalar) noexcept
    {
        float inv = 1.0f / scalar;
        return (*this) *= inv;
    }

    CRT_vector& operator-() noexcept
    {
        x=-x;
        y=-y;
        z=-z;
        return *this;
    }
    
// member fields
public:
    float x, y, z;
};


inline CRT_vector operator+ (const CRT_vector& left, const CRT_vector& right) noexcept 
{
    return CRT_vector(left.x + right.x, left.y + right.y, left.z + right.z);
}

inline CRT_vector operator- (const CRT_vector& left, const CRT_vector& right) noexcept 
{
    return CRT_vector(left.x - right.x, left.y - right.y, left.z - right.z);
}

// multiply vector by a scalar from the left
inline CRT_vector operator*(float scalar, const CRT_vector& v) noexcept 
{
    return CRT_vector(scalar * v.x, scalar * v.y, scalar * v.z);
}

// multiply vector by a scalar from the right
inline CRT_vector operator*(const CRT_vector& v, float scalar) noexcept 
{
    return scalar*v;
}

// dot broduct between two vectors
inline float dot(const CRT_vector& left, const CRT_vector& right) noexcept
{
    return (left.x * right.x) + (left.y * right.y) +(left.z * right.z);
}

// operator for dot broduct between two vectors
inline float operator*(const CRT_vector& left, const CRT_vector& right) noexcept
{
    return dot(left,right);
}

// divide a vector by a scalar
inline CRT_vector operator/(const CRT_vector& v, float scalar) noexcept
{
    float inv = 1.0f / scalar;
    return v * inv;
}

// cross product between two vectors
inline CRT_vector cross(const CRT_vector& left, const CRT_vector& right) noexcept
{
    return CRT_vector
    (
        left.y*right.z - left.z*right.y,
        left.z*right.x - left.x*right.z,
        left.x*right.y - left.y*right.x 
    );
}

// operator for cross product between two vectors
inline CRT_vector operator^(const CRT_vector& left, const CRT_vector& right) noexcept
{
    return cross(left,right);
}

inline CRT_vector CRT_vector::getNormalized() const noexcept
{
    return (*this) / length();
}

inline CRT_vector& CRT_vector::normalize() noexcept
{
    *this = *this / length();
    return *this;
}

inline std::ostream& operator<<(std::ostream& os, const CRT_vector& v)
{
    os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    return os;
}