#ifndef CRT_HIT_HPP
#define CRT_HIT_HPP

#include "CRT_triangle.hpp"
#include "CRT_mesh.hpp"

struct CRT_hit
{    
    float       t = 0.0f;
    CRT_vector point;
    CRT_vector barycentric;

    CRT_vector shading_normal;
    
    CRT_triangle triangle;
    
    int material_index = -1;
    
    CRT_hit() = default;
};


#endif