#ifndef CRT_LIGHT_HPP
#define CRT_LIGHT_HPP

#include "CRT_vector.hpp"

class CRT_light
{
public:
    CRT_light();
    CRT_light(const CRT_vector& _position, int _intensity);

    const CRT_vector& get_position() const;
    int get_intensity() const;
    
private:
    CRT_vector position;
    int intensity;
};


#endif