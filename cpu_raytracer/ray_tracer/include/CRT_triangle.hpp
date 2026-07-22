#include "CRT_vector.hpp"

static const int verts_in_triangle = 3;

class CRT_triangle
{
public: 
    CRT_triangle() = default;
    CRT_triangle(const CRT_vector& A, const CRT_vector& B, const CRT_vector& C) : verts {A,B,C}
    {
        CRT_vector  e0 = verts[1]-verts[0];
        CRT_vector  e1 = verts[2]-verts[0];
        normal_vector = (e0^e1).normalize();
    }

    CRT_vector verts[verts_in_triangle];
    CRT_vector normal_vector;
};