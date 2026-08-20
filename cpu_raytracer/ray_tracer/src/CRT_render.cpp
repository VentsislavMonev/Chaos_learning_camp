#include "CRT_render.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <limits>

CRT_render::CRT_render(const std::string& scene_file)
    : scene(scene_file)
    , settings(scene.get_settings())
    , camera(scene.get_camera())
    , objects(scene.get_objects())
    , textures(scene.get_textures())
    , materials(scene.get_materials())
    , lights(scene.get_lights())
{
}

bool CRT_render::intersect(const CRT_triangle& T, const CRT_ray& ray, CRT_hit& hit_point,
                            const CRT_vector& n0, const CRT_vector& n1, const CRT_vector& n2,
                            const CRT_vector& uv0, const CRT_vector& uv1, const CRT_vector& uv2) const
{
    float R_projection = T.normal_vector * ray.direction;
    float RT_distance  = T.normal_vector * (T.V0() - ray.origin);

    // checks if the ray and the plane of the triangle are parallel and if the ray is facing the plane
    if (fabs(R_projection) < 1e-6f) return false;

    // distance from ray origin to intersection point
    float t = RT_distance / R_projection;
    if (t <= 0) return false;

    // The point of intersection P
    CRT_vector P = ray.origin + t * ray.direction;

    CRT_vector V0_P = P - T.V0();
    CRT_vector V1_P = P - T.V1();
    CRT_vector V2_P = P - T.V2();

    // if P is inside the triangle
    if ( T.normal_vector*(T.E0()^V0_P) >= 0.0f &&
         T.normal_vector*(T.E1()^V1_P) >= 0.0f &&
         T.normal_vector*(T.E2()^V2_P) >= 0.0f)
    {
        // barycentric coords
        float denom = (T.E0()^T.E2()).length();
        float u = (V0_P^T.E2()).length() / denom;
        float v = (T.E0()^V0_P).length() / denom;
        float w = 1.0f - u - v;

        // give values to hit point
        hit_point.t                 = t;
        hit_point.point             = P;
        hit_point.barycentric       = CRT_vector(u, v, w);
        hit_point.triangle          = T;
        hit_point.shading_normal    = (n0 * w + n1 * u + n2 * v).normalize();
        hit_point.uv                = uv0 * w + uv1 * u + uv2 * v;

        return true;
    }
    return false;
}

bool CRT_render::intersect_shadow(const CRT_triangle& T, const CRT_ray& ray, float max_distance) const
{
    float R_projection = T.normal_vector * ray.direction;
    float RT_distance  = T.normal_vector * (T.V0() - ray.origin);

    if (fabs(R_projection) < 1e-6f) return false;

    float t = RT_distance / R_projection;
    if (t <= 0.0f || t >= max_distance) return false;

    CRT_vector P = ray.origin + t * ray.direction;

    CRT_vector V0_P = P - T.V0();
    CRT_vector V1_P = P - T.V1();
    CRT_vector V2_P = P - T.V2();

    return  T.normal_vector*(T.E0()^V0_P) >= 0.0f &&
            T.normal_vector*(T.E1()^V1_P) >= 0.0f &&
            T.normal_vector*(T.E2()^V2_P) >= 0.0f;
}

// Find closest intersection across the whole scene
bool CRT_render::intersect_scene(const CRT_ray& ray, CRT_hit& hit_point) const
{
    bool hit_anything = false;
    float closest_t = std::numeric_limits<float>::max();

    // for every object
    for (const CRT_mesh& mesh : objects)
    {
        size_t count = mesh.get_triangle_count();

        // for every triangle in the object
        for (size_t triangle_index = 0; triangle_index < count; triangle_index++)
        {
            // get vertices by triangle index
            CRT_vector v0, v1, v2;
            mesh.get_triangle_vertices(triangle_index, v0, v1, v2);
            CRT_triangle triangle(v0, v1, v2);

            // get vertices normals by triangle index
            CRT_vector n0, n1, n2;
            mesh.get_triangle_vertex_normals(triangle_index, n0, n1, n2);

            // get uvs
            CRT_vector uv0, uv1, uv2;
            mesh.get_triangle_uvs(triangle_index, uv0, uv1, uv2);

            // gets the closest hit_point to a triangle
            CRT_hit temp_hit;
            if (intersect(triangle, ray, temp_hit, n0, n1, n2, uv0, uv1, uv2) && temp_hit.t < closest_t)
            {
                closest_t = temp_hit.t;
                temp_hit.material_index = mesh.get_material_index();
                temp_hit.texture_index  = materials[temp_hit.material_index].texture_index;
                hit_point = temp_hit;
                hit_anything = true;
            }
        }
    }
    return hit_anything;
}

bool CRT_render::is_shadow(const CRT_ray& shadow_ray, float max_distance) const
{
    for (const CRT_mesh& mesh : objects)
    {
        // if refractive skip
        if (materials[mesh.get_material_index()].type == CRT_material_type::REFRACTIVE)
            continue;

        // checks if the shadow ray intersects any triangle in the scene

        size_t count = mesh.get_triangle_count();
        for (size_t i = 0; i < count; i++)
        {
            CRT_vector v0, v1, v2;
            mesh.get_triangle_vertices(i, v0, v1, v2);
            CRT_triangle triangle(v0, v1, v2);

            // checks if the shadow ray intersects this single triangle
            if (intersect_shadow(triangle, shadow_ray, max_distance))
                return true; // dont care about the closest hit just any hit
        }
    }
    return false;
}

CRT_vector CRT_render::shade_diffuse(const CRT_hit& hit_point) const
{
    const CRT_material& material    = materials[hit_point.material_index];
    const CRT_texture& texture      = textures[hit_point.texture_index];

    // get the normal vector that we will check depending if the smooth_shading flag is raised
    CRT_vector shading_normal = material.smooth_shading ? hit_point.shading_normal
                                                        : hit_point.triangle.normal_vector;

    CRT_vector result;

    // cycles through every light to check for shadows
    for (const CRT_light& light : lights)
    {
        // get light vector
        CRT_vector light_direction = light.get_position() - hit_point.point;

        // sphere for the light so we know if it traveled a lot or is near the hit point
        float sphere_radius = light_direction.length();
        light_direction.normalize();

        // create a shadow ray from the hit point + some shadow_bias for floating point error
        CRT_ray shadow_ray(hit_point.point + shading_normal * SHADOW_BIAS, light_direction);

        // check if the shadow ray is a shadow
        if (is_shadow(shadow_ray, sphere_radius))
            continue;

        // shade with the shading normal
        float cos_law = std::max(0.0f, light_direction * shading_normal);
        float distance_squared = sphere_radius * sphere_radius;

        // how much does a light contributes
        //
        // these numbers come from the surface area of a sphere:
        float contribution = (light.get_intensity() / (4.0f * 3.14159265f * distance_squared)) * cos_law * max_color_component;

        // tint by the materials albedo per-channel
        result += contribution * texture.sample(hit_point);
    }

    return CRT_vector(  std::min(max_color_component, result.x),
                        std::min(max_color_component, result.y),
                        std::min(max_color_component, result.z));
}

CRT_vector CRT_render::shade_reflective(const CRT_ray& ray, 
                                        const CRT_hit& hit_point,
                                        const CRT_vector& background, 
                                        int depth) const
{
    const CRT_material& material    = materials[hit_point.material_index];
    const CRT_texture& texture      = textures[hit_point.texture_index];

    // get the normal vector that we will check depending if the smooth_shading flag is raised
    CRT_vector shading_normal = material.smooth_shading
                                    ? hit_point.shading_normal
                                    : hit_point.triangle.normal_vector;

    // calculate reflected ray
    CRT_vector reflected_dir = ray.direction - shading_normal * (2.0f * (ray.direction * shading_normal));
    reflected_dir.normalize();

    // create a reflected ray from the hit point
    CRT_ray reflected_ray(  hit_point.point + shading_normal * SHADOW_BIAS,
                            reflected_dir);

    // trace reflected ray recursivly
    CRT_vector reflected_color = trace_ray(reflected_ray, background, depth + 1);

    // sample the color
    CRT_vector sampled_color = texture.sample(hit_point);

    // multiplying by albedo so the mirror can be seen and not blend in
    return CRT_vector(
        reflected_color.x * sampled_color.x,
        reflected_color.y * sampled_color.y,
        reflected_color.z * sampled_color.z
    );
}

CRT_vector CRT_render::shade_refraction(const CRT_ray& ray, 
                                        const CRT_hit& hit_point,
                                        const CRT_vector& background, 
                                        int depth) const
{
    // get the normal vector that we will check depending if the smooth_shading flag is raised
    const CRT_material& material = materials[hit_point.material_index];

    CRT_vector normal = material.smooth_shading
                        ? hit_point.shading_normal
                        : hit_point.triangle.normal_vector;

    // ior we are coming from and ior we are going into
    //
    // refractive materials cant get into each other because i havent implemented a way 
    // it turned out to be dificult

    float ior_from  = IOR_AIR;
    float ior_to    = material.ior;

    // if its exiting
    if(ray.direction*normal > 0.0f)
    {
        normal = -normal;
        ior_from = material.ior;
        ior_to = IOR_AIR;
    }

    // alpha = angle(ray.direction, normal)
    float cos_alpha = -(ray.direction * normal);
    float sin_alpha = sqrtf(1.0f - cos_alpha*cos_alpha);

    // reflected ray
    CRT_vector reflection_direction = ray.direction + 2 * (cos_alpha) * normal;
    CRT_ray reflection_ray(hit_point.point + (normal * REFRACTION_BIAS), reflection_direction);

    // angle is big enough for refraction and reflection
    if( sin_alpha < ior_to/ior_from)
    {
        // beta = angle(refracted_direction, -normal)
        float sin_beta = (sin_alpha * ior_from) / ior_to;
        float cos_beta = sqrtf(1 - sin_beta * sin_beta);

        // refracted ray
        CRT_vector refraction_direction = cos_beta * (-normal) +
                                          (ray.direction + cos_alpha * normal).getNormalized() * sin_beta;
        CRT_ray refraction_ray(hit_point.point + ((-normal)*REFRACTION_BIAS), refraction_direction);
        CRT_vector refracted_color = trace_ray(refraction_ray, background, depth + 1);

        // trace reflected ray
        CRT_vector reflected_color = trace_ray(reflection_ray, background, depth + 1);

        // get the color we need with fresnel
        float fresnel_constant = 0.5f * (1.0f - cos_alpha)*
                                        (1.0f - cos_alpha)*
                                        (1.0f - cos_alpha)*
                                        (1.0f - cos_alpha)*
                                        (1.0f - cos_alpha);

        return  (fresnel_constant * reflected_color + (1.0f - fresnel_constant) * refracted_color);
    }
    // angle is low enough for total internal reflection
    else
    {
        return trace_ray(reflection_ray, background, depth + 1);
    }
}

CRT_vector CRT_render::shade_constant(const CRT_hit& hit_point) const
{
    const CRT_texture& texture = textures[hit_point.texture_index];
    return texture.sample(hit_point) * max_color_component;
}

// Trace a single ray and return its color
CRT_vector CRT_render::trace_ray(const CRT_ray& ray, const CRT_vector& background, int depth) const
{
    // if max length is reached
    if (depth >= MAX_RAY_DEPTH)
        return background * max_color_component;

    // check if ray hitted something if not return background color
    CRT_hit hit_point;
    if (!intersect_scene(ray, hit_point))
        return background * max_color_component;

    // go throught material types
    switch (materials[hit_point.material_index].type)
    {
        // if diffuse
        case CRT_material_type::DIFFUSE:
        {
            return shade_diffuse(hit_point);
        }
        // if reflective
        case CRT_material_type::REFLECTIVE:
        {
            return shade_reflective(ray, hit_point, background, depth);
        }
        // if refractive
        // !!!
        // right now this currently works for just one transparent object and not nested ones
        // !!!
        case CRT_material_type::REFRACTIVE:
        {
            return shade_refraction(ray, hit_point, background, depth);
        }
        // if constant
        case CRT_material_type::CONSTANT:
        {
            return shade_constant(hit_point);
        }
    }
    return background * max_color_component;
}

void CRT_render::render(const std::string& output_file)
{
    // file we will write to
    std::ofstream ppm_file_stream(output_file);
    ppm_file_stream << "P3\n" << settings.image_width << " " << settings.image_height << "\n255\n";

    // go through each pixel
    for (int i = 0; i < settings.image_height; i++)
    {
        float y = camera.calculate_pixel_y(i);
        for (int j = 0; j < settings.image_width; j++)
        {
            float x = camera.calculate_pixel_x(j);
            CRT_ray ray = camera.generate_ray(x, y, -1.0f);

            CRT_vector color = trace_ray(ray, settings.background_color);

            ppm_file_stream << static_cast<int>(color.x) << " "
                            << static_cast<int>(color.y) << " "
                            << static_cast<int>(color.z) << "\t";
        }
        ppm_file_stream << '\n';
    }
}