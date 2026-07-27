#ifndef CRT_CAMERA_HPP
#define CRT_CAMERA_HPP

#include <math.h>
#include <stdexcept>
#include "CRT_vector.hpp"
#include "CRT_matrix.hpp"
#include "CRT_ray.hpp"

class CRT_camera
{
public:
    CRT_camera(const CRT_vector& _position, int _image_width, int _image_height);

// ray generation
public:

    float calculate_pixel_y(int i) const
    {
        float y = i + 0.5;
        y/=image_height;
        y = 1.0-2.0*y;
        return y;
    }

    float calculate_pixel_x(int j) const
    {
        float x = j + 0.5;
        x/=image_width;
        x = 2.0*x-1.0;
        x*= aspect_ratio;
        return x;
    }

    CRT_ray generate_ray(float pixel_x, float pixel_y, float pixel_z)
    {
        CRT_vector direction(pixel_x,pixel_y,pixel_z);
        direction = direction * rotation_matrix;
        direction.normalize();
        return CRT_ray(position,direction);
    }

// movement 
public:
    // translation 

    void truck(float distance);
    void dolly(float distance);
    void piedestal(float distance);
    void translate(const CRT_vector& move_direction);

    // rotation

    void pan(const float degrees);
    void tilt(const float degrees);
    void roll(const float degrees);

// getters 
public:
    CRT_vector get_position()const          {return position;}
    CRT_matrix get_rotation_matrix ()const  {return rotation_matrix;}
    int get_image_width()const              {return image_width;}
    int get_image_height()const             {return image_height;}
    float get_aspect_ratio()const           {return aspect_ratio;}

// setters 
public: 
    void set_position(const CRT_vector& _position);
    void set_image_width(int _image_width);
    void set_image_height(int _image_height);

    
private:
    void initialize_image_width(int _image_width);
    void initialize_image_height(int _image_height);

// members
private:
    CRT_vector position;
    CRT_matrix rotation_matrix;
    int image_width;
    int image_height;
    float aspect_ratio;
};
#endif