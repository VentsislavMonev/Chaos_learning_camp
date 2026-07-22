#include <fstream>
#include <random>
#include <vector>
#include "CRT_ray.hpp"
#include <algorithm>
#include <iostream>

/// Output image resolution
static const int image_width = 1920;
static const int image_height = 1080;
static const int max_color_component = 255;

// because we have the vector normalized the values are in the range [-1,1] so we cahnge it to [0,255] with this formula
int map_from_normalized_to_color(float old_value)
{
    // new_x = ((x-old_min)/(old_max-old_min)) * (new_max-new_min)+new_min
    // new_x = (x - (-1)) / (1 - (-1))*(255-0) + 0
    // new_x = (x+1)/2*255
    float new_value = (old_value + 1.0f) * 0.5f * static_cast<float>(max_color_component);

    // static_cast floors (removes everything after floating point)
    return static_cast<int>(new_value);
}

// Different way to map it
int map_from_normalized_to_color1(float old_value)
{
    old_value = std::abs(old_value);
    return fmod(old_value*1000.0f, 255.0f);
}

int main() 
{
    std::ofstream ppm_file_stream("generated_rays_direction.ppm", std::ios::out | std::ios::binary);
    ppm_file_stream << "P3\n";
    ppm_file_stream << image_width << " " << image_height << "\n";
    ppm_file_stream << max_color_component << "\n";

    std::vector<CRT_ray> rays;
    rays.reserve(image_height*image_width);

    for (int i = 0; i < image_height; i++)
    {
        float y = i + 0.5;
        y/=image_height;
        y = 1.0-2.0*y;

        for (int j = 0; j < image_width; j++)
        {
            float x = j + 0.5;

            x/=image_width;

            x = 2.0*x-1.0;

            x*= static_cast<float>(image_width)/static_cast<float>(image_height);

            CRT_vector direction(x,y,-1.0);
            CRT_vector origin(0,0,0);

            CRT_ray ray;
            ray.origin=origin;
            ray.direction=direction.normalize();

            rays.push_back(ray);
            
            int r = map_from_normalized_to_color1(ray.direction.x);
            int g = map_from_normalized_to_color1(ray.direction.y);
            int b = map_from_normalized_to_color1(ray.direction.z);

            ppm_file_stream << r << " " << g << " " << b << "\t";
        }
        ppm_file_stream << '\n';
    }
    std::cout<<rays.size();
}