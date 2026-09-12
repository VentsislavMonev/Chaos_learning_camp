#ifndef CRT_CAMERA_HPP
#define CRT_CAMERA_HPP

#include <math.h>
#include <numbers>
#include <stdexcept>
#include "CRT_vector3.hpp"
#include "CRT_matrix.hpp"
#include "CRT_ray.hpp"

class CRT_camera
{
public:
    CRT_camera();
    CRT_camera(const CRT_vector3& _position, int _image_width, int _image_height);
    CRT_camera(const CRT_vector3& _position, int _image_width, int _image_height, CRT_matrix matrix);

    // ray generation

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

    CRT_ray generate_ray(float pixel_x, float pixel_y, float pixel_z) const
    {
        CRT_vector3 direction(pixel_x,pixel_y,pixel_z);
        direction = direction * rotation_matrix;
        direction.normalize();
        return CRT_ray(position,direction);
    }

public:
    // translation 

    void truck(float distance);
    void dolly(float distance);
    void piedestal(float distance);
    void translate(const CRT_vector3& move_direction);

    // rotation

    void pan(float degrees);
    void tilt(float degrees);
    void roll(float degrees);

    // getters

    CRT_vector3 get_position()const          {return position;}
    CRT_matrix get_rotation_matrix ()const  {return rotation_matrix;}
    int get_image_width()const              {return image_width;}
    int get_image_height()const             {return image_height;}
    float get_aspect_ratio()const           {return aspect_ratio;}

    // setters

    void set_position(const CRT_vector3& _position);
    void set_image_width(int _image_width);
    void set_image_height(int _image_height);

    // if you want to create a camera with a given rotation from the start
    void set_rotation_matrix(const CRT_matrix& matrix);
    
private:
    void initialize_image_width(int _image_width);
    void initialize_image_height(int _image_height);

// members
private:
    CRT_vector3 position;
    CRT_matrix rotation_matrix;
    int image_width;
    int image_height;
    float aspect_ratio;
};
#endif