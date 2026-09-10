#ifndef CRT_TRIANGLE_HPP
#define CRT_TRIANGLE_HPP

#include "CRT_vector.hpp"

static const int verts_in_triangle = 3;

class CRT_triangle
{
    public: 
    CRT_triangle() = default;
    CRT_triangle(const CRT_vector3& A, const CRT_vector3& B, const CRT_vector3& C) noexcept : verts {A,B,C}
    {
        CRT_vector3  e0 = verts[1]-verts[0];
        CRT_vector3  e1 = verts[2]-verts[0];
        normal_vector = (e0^e1).normalize();
    }

    CRT_vector3 E0() const noexcept
    {
        return verts[1]-verts[0];
    }

    CRT_vector3 E1() const noexcept
    {
        return verts[2]-verts[1];
    }

    CRT_vector3 E2() const noexcept
    {
        return verts[0]-verts[2];
    }

    CRT_vector3 V0() const noexcept {return verts[0];}
    CRT_vector3 V1() const noexcept {return verts[1];}
    CRT_vector3 V2() const noexcept {return verts[2];}

    CRT_vector3 verts[verts_in_triangle];
    CRT_vector3 normal_vector;
};

#endif