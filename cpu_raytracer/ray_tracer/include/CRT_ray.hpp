#ifndef CRT_RAY_HPP
#define CRT_RAY_HPP
#include "CRT_vector.hpp"

struct CRT_ray
{
    CRT_vector origin;
    CRT_vector direction;
};

inline float calculate_y(int i, int image_height)
{
    // float y = i + 0.5;
    // y/=image_height;
    // y = 1.0-2.0*y;
    return 1.0-2.0*((i + 0.5)/image_height);
}

inline float calculate_x(int j, int image_width, float aspect_ratio)
{
    // float x = j + 0.5;
    // x/=image_width;
    // x = 2.0*x-1.0;
    // x*= aspect_ratio;
    return (2.0*((j+0.5)/image_width) - 1.0)*aspect_ratio;
}

CRT_ray generate_ray(float x, float y, float z)
{
    CRT_vector direction(x,y,z);
    CRT_vector origin(0,0,0);
    CRT_ray res;
    res.direction=direction;
    res.origin=origin;
    return res;
}

#endif