#ifndef CRT_RAY_HPP
#define CRT_RAY_HPP
#include "CRT_vector.hpp"

struct CRT_ray
{
    CRT_ray(const CRT_vector& _origin, const CRT_vector& _dircetion)
    {
        origin=_origin;
        direction=_dircetion;
    }
    
    CRT_vector origin;
    CRT_vector direction;
};

#endif