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
    if( T.normal_vector*(T.E0()^V0_P)>0 &&
        T.normal_vector*(T.E1()^V1_P)>0 &&
        T.normal_vector*(T.E2()^V2_P)>0)
    {
        out_t=t;
        return true;
    }
    else return false;

}

int main() 
{
    // ---- Pyramid -------------------------------------
    
    // CRT_vector apex ( 0.5f,  1.0f, -4.3f);
    // CRT_vector B0   (-0.37f, -1.0f, -2.63f);
    // CRT_vector B1   ( 1.37f, -1.0f, -3.63f);
    // CRT_vector B2   ( 0.37f, -1.0f, -5.37f);
    // CRT_vector B3   (-1.37f, -1.0f, -4.37f);
 
    // std::vector<CRT_triangle> triangles =
    // {
    //     // 4 triangular side faces
    //     { apex, B2, B3 },
    //     { apex, B1, B2 },
    //     { apex, B0, B1 },
    //     { apex, B3, B0 },
 
    //     // base, split into 2 triangles
    //     { B0, B2, B1 },
    //     { B0, B3, B2 }        
    // };
 
    // std::vector<CRT_vector> colors =
    // {
    //     {255,  0,  0},
    //     {  0,255,  0},
    //     {  0,  0,255},
    //     {255,255,  0},
    //     {128,128,128},
    //     {255,255,255}
    // };

    // ---- star with triangles -------------------------------------
    
    CRT_vector center(0.0f, 0.0f, -3.0f);

    CRT_vector V0(0.0000f,  1.7500f, -3);
    CRT_vector V1(0.3927f,  0.5403f, -3);
    CRT_vector V2(1.6644f,  0.5407f, -3);
    CRT_vector V3(0.6353f, -0.2065f, -3);
    CRT_vector V4(1.0287f, -1.4158f, -3);
    CRT_vector V5(0.0000f, -0.6683f, -3);
    CRT_vector V6(-1.0287f,-1.4158f, -3);
    CRT_vector V7(-0.6353f,-0.2065f, -3);
    CRT_vector V8(-1.6644f, 0.5407f, -3);
    CRT_vector V9(-0.3927f, 0.5403f, -3);

    std::vector<CRT_triangle> triangles =
    {
        { center, V1, V0 },
        { center, V2, V1 },
        { center, V3, V2 },
        { center, V4, V3 },
        { center, V5, V4 },
        { center, V6, V5 },
        { center, V7, V6 },
        { center, V8, V7 },
        { center, V9, V8 },
        { center, V0, V9 }
    };

    std::vector<CRT_vector> colors = 
    {
        {0,0,0},
        {128,0,0},
        {0,128,0},
        {128,128,0},
        {0,0,128},
        {128,0,128},
        {0,128,128},
        {255,0,0},
        {0,255,0},
        {0,0,255} 
    };




    for (size_t frame = 0; frame < 72; frame++)
    {
        CRT_vector camera_origin(0,0,0);
        CRT_camera camera(camera_origin, image_width, image_height);

        camera.roll(5*frame);

        std::ostringstream filename;
        filename << "frame_" << std::setw(3) << std::setfill('0') << frame << ".ppm";

        std::ofstream ppm_file_stream(filename.str(), std::ios::out | std::ios::binary);
        ppm_file_stream << "P3\n";
        ppm_file_stream << image_width << " " << image_height << "\n";
        ppm_file_stream << max_color_component << "\n";

        size_t triangles_count = triangles.size();
        float y = 0.0;
        float x = 0.0;
        
        for (int i = 0; i < image_height; i++)
        {   
            y = camera.calculate_pixel_y(i);
            for (int j = 0; j < image_width; j++)
            {
                x = camera.calculate_pixel_x(j);
                CRT_ray ray = camera.generate_ray(x,y,-1.0f);

                int r = 10, g = 10, b = 10;
                float closest_t = std::numeric_limits<float>::max();

                for (size_t c = 0; c < triangles_count; c++)
                {
                    float t;
                    if (intersect(triangles[c], ray,t))
                    {
                        if(t<closest_t) 
                        {
                            closest_t = t;
                            r = colors[c].x;
                            g = colors[c].y;
                            b = colors[c].z;
                        }
                    }
                }

                ppm_file_stream << r << " " << g << " " << b << "\t";
            }
            ppm_file_stream << '\n';
        }
    }
}