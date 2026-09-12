#ifndef CRT_TRIANGLE_HPP
#define CRT_TRIANGLE_HPP

#include "CRT_vector3.hpp"

static const int verts_in_triangle = 3;

class CRT_triangle
{
    public: 
    CRT_triangle() = default;
    CRT_triangle(const CRT_vector3& A, const CRT_vector3& B, const CRT_vector3& C) noexcept : verts {A,B,C}
    {
        edge0 = verts[1]-verts[0];
        edge1 = verts[2]-verts[1];
        edge2 = verts[0]-verts[2];

        CRT_vector3 raw_normal  = edge0 ^ edge1;
        double_area             = raw_normal.length();
        inv_double_area         = 1.0f / double_area;
        normal_vector           = raw_normal * inv_double_area;
    
    }

    CRT_vector3 V0() const noexcept {return verts[0];}
    CRT_vector3 V1() const noexcept {return verts[1];}
    CRT_vector3 V2() const noexcept {return verts[2];}

    CRT_vector3 verts[verts_in_triangle];
    CRT_vector3 normal_vector;
    CRT_vector3 edge0, edge1, edge2;
    float double_area     = 0.0f;
    float inv_double_area = 0.0f;
};

#endif