#ifndef CRT_RAY_HPP
#define CRT_RAY_HPP
#include "CRT_vector3.hpp"

struct CRT_ray
{
    CRT_ray(const CRT_vector3& _origin, const CRT_vector3& _dircetion)
    {
        origin=_origin;
        direction=_dircetion;
    }
    
    CRT_vector3 origin;
    CRT_vector3 direction;
};

#endif