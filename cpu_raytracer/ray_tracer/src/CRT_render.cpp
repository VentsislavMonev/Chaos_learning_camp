#include "CRT_render.hpp"


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

bool CRT_render::intersect(const CRT_triangle& T, const CRT_ray& ray, 
                           float t_max, float& t_out, float& w_out, float& u_out)const
{
    const CRT_vector3& normal = T.normal_vector;
    
    float R_projection = normal * ray.direction;

    // checks if the ray and the plane of the triangle are parallel 
    if (fabs(R_projection) < 1e-6f) return false;

    float RT_distance  = normal * (T.V0() - ray.origin);

    // distance from ray origin to intersection point
    float t = RT_distance / R_projection;
    if (t <= 0.0f || t >= t_max) return false;

    // The point of intersection P
    CRT_vector3 P = ray.origin + t * ray.direction;

    // Vectors from each triangle vertex to the hit point P.
    CRT_vector3 V0_P = P - T.V0();
    CRT_vector3 V1_P = P - T.V1();
    CRT_vector3 V2_P = P - T.V2();

    // if P is inside the triangle
    float d0 = normal * (T.edge0 ^ V0_P);
    if (!(d0 >= 0.0f)) return false;
    
    float d1 = normal * (T.edge1 ^ V1_P);
    if (!(d1 >= 0.0f)) return false;

    float d2 = normal * (T.edge2 ^ V2_P);
    if (!(d2 >= 0.0f)) return false;
 
    // keep the best values
    t_out = t;
    w_out = d1 * T.inv_double_area;
    u_out = d2 * T.inv_double_area;
    
    return true;
}

// Find closest intersection across the whole scene
bool CRT_render::intersect_scene(const CRT_ray& ray, CRT_hit& hit_point) const
{
    const CRT_mesh* best_mesh = nullptr;
    size_t best_index = 0;
    float closest_t = std::numeric_limits<float>::max();
    float best_w = 0.0f;
    float best_u = 0.0f;

    // for every object
    for (const CRT_mesh& mesh : objects)
    {
        const std::vector<CRT_triangle>& triangles = mesh.get_precomputed_triangles();
        size_t count = mesh.get_triangle_count();

        // for every triangle in the object
        for (size_t i = 0; i < count; ++i)
        {
            float t, w, u;

            if (intersect(triangles[i], ray, closest_t, t, w, u))
            {
                closest_t = t;
                best_mesh = &mesh;
                best_index = i;
                best_w = w;
                best_u = u;
            }
        }
    }

    // if no mesh was hit
    if (!best_mesh) return false;

    // closest triangle hit
    const CRT_triangle& triangle = best_mesh->get_precomputed_triangles()[best_index];

    CRT_vector3 n0, n1, n2, uv0, uv1, uv2;
    best_mesh->get_triangle_vertex_normals(best_index, n0, n1, n2);
    best_mesh->get_triangle_uvs(best_index, uv0, uv1, uv2);

    // barycentric coordinates
    const float w = best_w;
    const float u = best_u;
    const float v = 1.0f - u - w;

    hit_point.t               = closest_t;
    hit_point.point           = ray.origin + closest_t * ray.direction;
    hit_point.barycentric     = CRT_vector3(u, v, w);
    hit_point.triangle        = triangle;
    hit_point.shading_normal  = (n0 * w + n1 * u + n2 * v).normalize();
    hit_point.uv              = uv0 * w + uv1 * u + uv2 * v;
    hit_point.material_index  = best_mesh->get_material_index();
    hit_point.texture_index   = materials[hit_point.material_index].texture_index;
    return true;
}

bool CRT_render::is_shadow(const CRT_ray& shadow_ray, float t_max) const
{
    for (const CRT_mesh& mesh : objects)
    {
        // if refractive skip
        if (materials[mesh.get_material_index()].type == CRT_material_type::REFRACTIVE)
            continue;

        // checks if the shadow ray intersects any triangle in the scene

        // place holders for the intersect function
        float t, w, u;

        const std::vector<CRT_triangle>& triangles = mesh.get_precomputed_triangles();

        for (const CRT_triangle& triangle : triangles)
        {
            // checks if the shadow ray intersects this single triangle
            if (intersect(triangle, shadow_ray, t_max, t, w, u))
                return true; // dont care about the closest hit just any hit
        }
    }
    return false;
}

CRT_vector3 CRT_render::shade_diffuse(const CRT_hit& hit_point) const
{
    const CRT_material& material    = materials[hit_point.material_index];
    const CRT_texture& texture      = textures[hit_point.texture_index];

    // get the normal vector that we will check depending if the smooth_shading flag is raised
    CRT_vector3 shading_normal = material.smooth_shading ? hit_point.shading_normal
                                                        : hit_point.triangle.normal_vector;

    CRT_vector3 result;

    // cycles through every light to check for shadows
    for (const CRT_light& light : lights)
    {
        // get light vector
        CRT_vector3 light_direction = light.get_position() - hit_point.point;

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
        float contribution = (light.get_intensity() / (4.0f * std::numbers::pi_v<float> * distance_squared)) * cos_law * max_color_component;

        // tint by the materials albedo per-channel
        result += contribution * texture.sample(hit_point);
    }

    return CRT_vector3(  std::min(max_color_component, result.x),
                        std::min(max_color_component, result.y),
                        std::min(max_color_component, result.z));
}

CRT_vector3 CRT_render::shade_reflective(const CRT_ray& ray, 
                                        const CRT_hit& hit_point,
                                        const CRT_vector3& background, 
                                        int depth) const
{
    const CRT_material& material    = materials[hit_point.material_index];
    const CRT_texture& texture      = textures[hit_point.texture_index];

    // get the normal vector that we will check depending if the smooth_shading flag is raised
    CRT_vector3 shading_normal = material.smooth_shading
                                    ? hit_point.shading_normal
                                    : hit_point.triangle.normal_vector;

    // calculate reflected ray
    CRT_vector3 reflected_dir = ray.direction - shading_normal * (2.0f * (ray.direction * shading_normal));
    reflected_dir.normalize();

    // create a reflected ray from the hit point
    CRT_ray reflected_ray(  hit_point.point + shading_normal * SHADOW_BIAS,
                            reflected_dir);

    // trace reflected ray recursivly
    CRT_vector3 reflected_color = trace_ray(reflected_ray, background, depth + 1);

    // sample the color
    CRT_vector3 sampled_color = texture.sample(hit_point);

    // multiplying by albedo so the mirror can be seen and not blend in
    return CRT_vector3(
        reflected_color.x * sampled_color.x,
        reflected_color.y * sampled_color.y,
        reflected_color.z * sampled_color.z
    );
}

CRT_vector3 CRT_render::shade_refraction(const CRT_ray& ray, 
                                        const CRT_hit& hit_point,
                                        const CRT_vector3& background, 
                                        int depth) const
{
    // get the normal vector that we will check depending if the smooth_shading flag is raised
    const CRT_material& material = materials[hit_point.material_index];

    CRT_vector3 normal = material.smooth_shading
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
    CRT_vector3 reflection_direction = ray.direction + 2 * (cos_alpha) * normal;
    CRT_ray reflection_ray(hit_point.point + (normal * REFRACTION_BIAS), reflection_direction);

    // angle is big enough for refraction and reflection
    if( sin_alpha < ior_to/ior_from)
    {
        // beta = angle(refracted_direction, -normal)
        float sin_beta = (sin_alpha * ior_from) / ior_to;
        float cos_beta = sqrtf(1 - sin_beta * sin_beta);

        // refracted ray
        CRT_vector3 refraction_direction = cos_beta * (-normal) +
                                          (ray.direction + cos_alpha * normal).getNormalized() * sin_beta;
        CRT_ray refraction_ray(hit_point.point + ((-normal)*REFRACTION_BIAS), refraction_direction);
        CRT_vector3 refracted_color = trace_ray(refraction_ray, background, depth + 1);

        // trace reflected ray
        CRT_vector3 reflected_color = trace_ray(reflection_ray, background, depth + 1);

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

CRT_vector3 CRT_render::shade_constant(const CRT_hit& hit_point) const
{
    const CRT_texture& texture = textures[hit_point.texture_index];
    return texture.sample(hit_point) * max_color_component;
}

// Trace a single ray and return its color
CRT_vector3 CRT_render::trace_ray(const CRT_ray& ray, const CRT_vector3& background, int depth) const
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





void CRT_render::render_bucket(const CRT_bucket& bucket, std::vector<CRT_vector3>& image_buffer) const
{
    for (int i = bucket.y0; i < bucket.y1; ++i)
    {
        float y = camera.calculate_pixel_y(i);
        for (int j = bucket.x0; j < bucket.x1; ++j)
        {
            float x = camera.calculate_pixel_x(j);
            CRT_ray ray = camera.generate_ray(x, y, -1.0f);

            // each pixel belongs to exactly one bucket, so no locking is needed
            image_buffer[static_cast<size_t>(i) * settings.image_width + j] =
                trace_ray(ray, settings.background_color);
        }
    }
}

void CRT_render::write_buffer_to_file(const std::string &output_file, const std::vector<CRT_vector3> &image_buffer) const
{
    const int width  = settings.image_width;
    const int height = settings.image_height;
    
    std::ofstream file(output_file);
    if(!file) throw std::runtime_error("Failed to open output file: " + output_file);
    
    file << "P3\n" << width << ' ' << height << "\n" << static_cast<int>(max_color_component) << "\n";
    
    std::string row;
    const int int_size_times_3 = 12;
    row.reserve(static_cast<size_t>(width) * int_size_times_3);

    std::function<int(float)> to_byte = [](float c) { return std::clamp(static_cast<int>(c), 0, static_cast<int>(max_color_component)); };
    
    for (int i = 0; i < height; ++i)
    {
        row.clear();

        for (int j = 0; j < width; ++j)
        {
            const CRT_vector3& c = image_buffer[static_cast<size_t>(i) * width + j];

            row += std::to_string(to_byte(c.x)) + ' '
                 + std::to_string(to_byte(c.y)) + ' '
                 + std::to_string(to_byte(c.z)) + '\t';
        }

        row += '\n';
        file << row;
    }
    
    file.close();
    if(!file) throw std::runtime_error("Failed to close output file: " + output_file);
    
}

void CRT_render::render(const std::string& output_file)
{
    std::vector<CRT_vector3> image_buffer(static_cast<size_t>(settings.image_width) * settings.image_height);
    const std::vector<CRT_bucket> buckets = CRT_make_buckets(settings.image_width, settings.image_height);

    // render every bucket on the pool
    {
        CRT_thread_pool pool;
        for (const CRT_bucket& bucket : buckets)
            pool.enqueue([this, bucket, &image_buffer] { render_bucket(bucket, image_buffer); });
        pool.wait();
    }

    write_buffer_to_file(output_file, image_buffer);
}