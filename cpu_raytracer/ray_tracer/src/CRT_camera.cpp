#include "CRT_camera.hpp"

void CRT_camera::initialize_image_width(int _image_width)
{
    if(_image_width<=0) throw std::invalid_argument("Image width must be a positive number!");
    image_width = _image_width;
}

void CRT_camera::initialize_image_height(int _image_height)
{
    if(_image_height<=0) throw std::invalid_argument("Image height must be a positive number!");
    image_height = _image_height;
}

void CRT_camera::set_position(const CRT_vector &_position)
{
    position = _position;
}

void CRT_camera::set_image_width(int _image_width)
{
    if(_image_width <= 0) throw std::invalid_argument("Image width must be a positive number!");
    image_width = _image_width;
    aspect_ratio = static_cast<float>(image_width)/static_cast<float>(image_height);
}

void CRT_camera::set_image_height(int _image_height)
{
    if(_image_height <= 0) throw std::invalid_argument("Image height must be a positive number!");
    image_height = _image_height;
    aspect_ratio = static_cast<float>(image_width)/static_cast<float>(image_height);
}

void CRT_camera::set_rotation_matrix(const CRT_matrix &matrix)
{
    rotation_matrix=matrix;
}

CRT_camera::CRT_camera()
{
    position = CRT_vector();
    rotation_matrix = CRT_matrix();
    image_width=1;
    image_height=1;
    aspect_ratio=1;
}

CRT_camera::CRT_camera(const CRT_vector &_position, int _image_width, int _image_height)
{
    set_position(_position);
    initialize_image_width(_image_width);
    initialize_image_height(_image_height);
    aspect_ratio = static_cast<float>(image_width)/static_cast<float>(image_height);
}

CRT_camera::CRT_camera(const CRT_vector &_position, int _image_width, int _image_height, CRT_matrix matrix)
{
    set_position(_position);
    initialize_image_width(_image_width);
    initialize_image_height(_image_height);
    set_rotation_matrix(matrix);
    aspect_ratio = static_cast<float>(image_width)/static_cast<float>(image_height);
}


void CRT_camera::truck(float distance)
{
    CRT_vector new_x(distance,0,0);
    new_x = new_x*rotation_matrix;
    position+=new_x;
}

void CRT_camera::dolly(float distance)
{
    CRT_vector new_z(0,0, distance);
    new_z = new_z*rotation_matrix;
    position+=new_z;
}

void CRT_camera::piedestal(float distance)
{
    CRT_vector new_y(0, distance,0);
    new_y = new_y*rotation_matrix;
    position+=new_y;
}

void CRT_camera::translate(const CRT_vector &move_direction)
{
    CRT_vector move_direction_world_space = move_direction*rotation_matrix;
    position+=move_direction_world_space;
}


void CRT_camera::pan(const float degrees)
{
    const float rads = degrees * (M_PIf/180.f);
    const CRT_matrix rotate_around_y = 
    {
        cosf(rads)  , 0.f   , -sinf(rads),
        0.f         , 1.f   , 0.f,
        sinf(rads)  , 0.f   , cosf(rads) 
    };

    rotation_matrix = rotation_matrix*rotate_around_y;
}

void CRT_camera::tilt(const float degrees)
{
    const float rads = degrees * (M_PIf/180.f);
    const CRT_matrix rotate_around_x = 
    {
        1.f   ,   0.f      , 0.f,
        0.f   , cosf(rads)  , sinf(rads),
        0.f   , -sinf(rads) , cosf(rads)
    };

    rotation_matrix = rotation_matrix*rotate_around_x;
}

void CRT_camera::roll(const float degrees)
{
    const float rads = degrees * (M_PIf/180.f);
    const CRT_matrix rotate_around_z = 
    {
        cosf(rads)  , sinf(rads), 0.f,
        -sinf(rads) , cosf(rads), 0.f,
        0.f        ,    0.f   , 1.f
    };

    rotation_matrix = rotation_matrix*rotate_around_z;
}
