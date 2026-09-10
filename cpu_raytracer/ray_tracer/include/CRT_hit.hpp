#ifndef CRT_HIT_HPP
#define CRT_HIT_HPP

#include "CRT_triangle.hpp"
#include "CRT_mesh.hpp"

struct CRT_hit
{    
    float t = 0.0f;
    CRT_vector3 point;
    CRT_vector3 barycentric;
    CRT_vector3 uv;
    CRT_vector3 shading_normal;
    CRT_triangle triangle;
    
    int material_index  = -1;
    int texture_index   = -1;
    
    CRT_hit() = default;
};


#endif