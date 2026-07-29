#include <fstream>
#include <random>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "CRT_ray.hpp"
#include "CRT_triangle.hpp"
#include "CRT_camera.hpp"
#include "CRT_scene.hpp"

/// Output image resolution
static const int image_width = 1920;
static const int image_height = 1080;
static const int max_color_component = 255;

static const float aspect_ratio = static_cast<float>(image_width)/static_cast<float>(image_height);

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

inline bool intersect(const CRT_triangle& T, const CRT_ray& R, float& out_t)
{
    float R_projection = T.normal_vector*R.direction;

    // distane from ray origin to triangle plane
    float RT_distance = T.normal_vector * (T.V0() - R.origin);

    // checks if the ray and the plane of the triangle are parallel and if the ray is facing the plane
    if(fabs(R_projection)<1e-6f || RT_distance >=0.0f) return false;

    // distance from ray origin to Intersection point
    float t = RT_distance/R_projection;
    if(t<=0) return false;

    // The point of intersection P
    CRT_vector P = R.origin + t* R.direction;

    CRT_vector V0_P = P-T.V0();
    CRT_vector V1_P = P-T.V1();
    CRT_vector V2_P = P-T.V2();

    // if P inside the triangle
    if( T.normal_vector*(T.E0()^V0_P)>=0.0f &&
        T.normal_vector*(T.E1()^V1_P)>=0.0f &&
        T.normal_vector*(T.E2()^V2_P)>=0.0f)
    {
        out_t=t;
        return true;
    }
    else return false;

}

int main() 
{
    CRT_scene scene("../scene4.crtscene");

    const CRT_settings& settings            = scene.get_settings();
    const CRT_camera& camera                = scene.get_camera();
    const std::vector<CRT_mesh>& objects    = scene.get_objects();

    int image_width  = settings.image_width;
    int image_height = settings.image_height;

    CRT_vector background = settings.background_color;

    std::ofstream ppm_file_stream("output.ppm");
    ppm_file_stream << "P3\n" << image_width << " " << image_height << "\n255\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);    

    int max_numb_triangles_in_meshes = 1;

    for (const CRT_mesh& mesh : objects)
    {
        if(mesh.get_triangle_count()>max_numb_triangles_in_meshes)
            max_numb_triangles_in_meshes = mesh.get_triangle_count();
    }
    
    std::vector<CRT_vector> colors;
    colors.reserve(max_numb_triangles_in_meshes);

    for (size_t i = 0; i < max_numb_triangles_in_meshes; i++)
    {
        int random_number1 = dist(gen);
        int random_number2 = dist(gen);
        int random_number3 = dist(gen);
        colors.push_back(CRT_vector(random_number1,random_number2,random_number3));
    }
    

    for (int i = 0; i < image_height; i++)
    {
        float y = camera.calculate_pixel_y(i);
        for (int j = 0; j < image_width; j++)
        {
            float x = camera.calculate_pixel_x(j);
            CRT_ray ray = camera.generate_ray(x, y, -1.0f);

            int r = static_cast<int>(background.x * 255);
            int g = static_cast<int>(background.y * 255);
            int b = static_cast<int>(background.z * 255);
            
            float closest_t = std::numeric_limits<float>::max();
            
            for (const CRT_mesh& mesh : objects)
            {
                for (size_t triangle_index = 0; triangle_index < mesh.get_triangle_count(); triangle_index++)
                {
                    float t;

                    CRT_vector v0;
                    CRT_vector v1;
                    CRT_vector v2;

                    mesh.get_triangle_vertices(triangle_index, v0 , v1, v2);

                    CRT_triangle triangle(v0,v1,v2);

                    if (intersect(triangle, ray,t))
                    {
                        if(t<closest_t) 
                        {
                            closest_t = t;

                            // coloring based on normal vector
                            CRT_vector color(
                                (triangle.normal_vector.x + 1.0f) * 127.5f,
                                (triangle.normal_vector.y + 1.0f) * 127.5f,
                                (triangle.normal_vector.z + 1.0f) * 127.5f
                            );
                            
                            r = color.x;
                            g = color.y;
                            b = color.z;
                        }
                    }
                }
            }

            ppm_file_stream << r << " " << g << " " << b << "\t";
        }
        ppm_file_stream << '\n';
    }
}