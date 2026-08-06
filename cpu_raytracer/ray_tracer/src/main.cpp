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
#include "CRT_light.hpp"

/// Output image resolution
static const int image_width = 1920;
static const int image_height = 1080;
static const int max_color_component = 255;
static const float shadow_bias = 1e-4f;

inline bool intersect(const CRT_triangle& T, const CRT_ray& R, float& out_t)
{
    float R_projection = T.normal_vector*R.direction;

    // distane from ray origin to triangle plane
    float RT_distance = T.normal_vector * (T.V0() - R.origin);

    // checks if the ray and the plane of the triangle are parallel and if the ray is facing the plane
    if(fabs(R_projection)<1e-6f) return false;

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

// Find closest intersection across the whole scene
bool intersect_scene(const std::vector<CRT_mesh>& objects, const CRT_ray& ray, float& out_t, CRT_triangle& out_triangle)
{
    bool hit_anything = false;
    float closest_t = std::numeric_limits<float>::max();

    for (const CRT_mesh& mesh : objects)
    {
        for (size_t i = 0; i < mesh.get_triangle_count(); i++)
        {
            CRT_vector v0, v1, v2;
            mesh.get_triangle_vertices(i, v0, v1, v2);
            CRT_triangle triangle(v0, v1, v2);

            float t;
            if (intersect(triangle, ray, t) && t < closest_t)
            {
                closest_t = t;
                out_triangle = triangle;
                hit_anything = true;
            }
        }
    }

    out_t = closest_t;
    return hit_anything;
}

bool is_shadow(const CRT_ray& shadow_ray, const std::vector<CRT_mesh>& objects, float max_distance)
{
    for (const CRT_mesh& mesh : objects)
    {
        for (size_t i = 0; i < mesh.get_triangle_count(); i++)
        {
            CRT_vector v0, v1, v2;
            mesh.get_triangle_vertices(i, v0, v1, v2);
            CRT_triangle triangle(v0, v1, v2);

            float t;
            if (intersect(triangle, shadow_ray, t) && t < max_distance)
            {
                return true; // something's in the way, stop immediately
            }
        }
    }
    return false;
}

// Compute shading at a hit point from all lights
CRT_vector shade_point( const CRT_vector& hit_point,
                        const CRT_triangle& triangle,
                        const std::vector<CRT_light>& lights,
                        const std::vector<CRT_mesh>& objects)
{
    CRT_vector albedo(0.0f, 0.0f, 1.0f);
    float red = 0.0f, green = 0.0f, blue = 0.0f;

    for (const CRT_light& light : lights)
    {
        CRT_vector light_direction = light.get_position() - hit_point;
        float sphere_radius = light_direction.length();
        light_direction.normalize();

        CRT_ray shadow_ray(hit_point + triangle.normal_vector * shadow_bias, light_direction);

        if(is_shadow(shadow_ray, objects, sphere_radius)) 
            continue;
        
        float cos_law = std::max(0.0f, light_direction * triangle.normal_vector);
        
        float distance_squared = sphere_radius * sphere_radius;
        
        float contribution = (light.get_intensity() / (4.0f *  3.14159265f * distance_squared)) * 
                              cos_law * 
                              255.0f;

        red   += contribution * albedo.x;
        green += contribution * albedo.y;
        blue  += contribution * albedo.z;
    }

    return CRT_vector(red, green, blue);
}

// Trace a single ray and return its color
CRT_vector trace_ray(const CRT_ray& ray, const std::vector<CRT_mesh>& objects, const std::vector<CRT_light>& lights, const CRT_vector& background)
{
    float t;
    CRT_triangle triangle; // placeholder, adjust to your ctor

    if (!intersect_scene(objects, ray, t, triangle))
        return background * 255.0f; // or however you scale background

    CRT_vector hit_point = ray.origin + ray.direction * t;
    CRT_vector color = shade_point(hit_point, triangle, lights, objects);

    return CRT_vector(
        std::min((float)max_color_component, color.x),
        std::min((float)max_color_component, color.y),
        std::min((float)max_color_component, color.z)
    );
}


int main()
{
    CRT_scene scene("../scene3.crtscene");
    const CRT_settings& settings = scene.get_settings();
    const CRT_camera& camera = scene.get_camera();
    const std::vector<CRT_mesh>& objects = scene.get_objects();
    const std::vector<CRT_light>& lights = scene.get_lights();

    std::ofstream ppm_file_stream("output3.ppm");
    ppm_file_stream << "P3\n" << settings.image_width << " " << settings.image_height << "\n255\n";

    for (int i = 0; i < settings.image_height; i++)
    {
        float y = camera.calculate_pixel_y(i);
        for (int j = 0; j < settings.image_width; j++)
        {
            float x = camera.calculate_pixel_x(j);
            CRT_ray ray = camera.generate_ray(x, y, -1.0f);

            CRT_vector color = trace_ray(ray, objects, lights, settings.background_color);

            ppm_file_stream << static_cast<int>(color.x) << " "
                            << static_cast<int>(color.y) << " "
                            << static_cast<int>(color.z) << "\t";
        }
        ppm_file_stream << '\n';
    }
}