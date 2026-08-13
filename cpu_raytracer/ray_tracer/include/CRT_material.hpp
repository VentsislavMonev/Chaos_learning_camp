#ifndef CRT_MATERIAL_HPP
#define CRT_MATERIAL_HPP

#include "CRT_vector.hpp"

enum class CRT_material_type
{
    DIFFUSE,
    REFLECTIVE,
    REFRACTIVE,
    CONSTANT
};

struct CRT_material
{
    CRT_material_type type;
    CRT_vector albedo;
    float ior;
    bool smooth_shading;
};


#endif