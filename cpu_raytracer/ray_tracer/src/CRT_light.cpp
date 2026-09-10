#include "CRT_light.hpp"

CRT_light::CRT_light() : position(CRT_vector3()), intensity(0) {}

CRT_light::CRT_light(const CRT_vector3 &_position, int _intensity) : position(_position), intensity(_intensity) {}

const CRT_vector3 &CRT_light::get_position() const
{
    return position;
}

int CRT_light::get_intensity() const
{
    return intensity;
}
