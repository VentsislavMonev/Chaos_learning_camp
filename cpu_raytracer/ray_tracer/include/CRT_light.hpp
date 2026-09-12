#ifndef CRT_LIGHT_HPP
#define CRT_LIGHT_HPP

#include "CRT_vector3.hpp"

class CRT_light
{
public:
    CRT_light();
    CRT_light(const CRT_vector3& _position, int _intensity);

    const CRT_vector3& get_position() const;
    int get_intensity() const;
    
private:
    CRT_vector3 position;
    int intensity;
};


#endif