#ifndef CRT_RENDERER_HPP
#define CRT_RENDERER_HPP


#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>
#include "CRT_ray.hpp"
#include "CRT_triangle.hpp"
#include "CRT_camera.hpp"
#include "CRT_scene.hpp"
#include "CRT_light.hpp"
#include "CRT_hit.hpp"
#include "CRT_texture.hpp"
#include "CRT_thread_pool.hpp"
#include "CRT_bucket.hpp"

class CRT_render
{
public:
    explicit CRT_render(const std::string& scene_file);

    // Runs the full render loop and writes the result to output_file as a PPM image.
    void render(const std::string& output_file);
    void render_(const std::string& output_file);

// intersection functions
private:
    // low level triangle intersection
    bool intersect( const CRT_triangle& T, const CRT_ray& ray, CRT_hit& hit_point,
                    const CRT_vector3& n0, const CRT_vector3& n1, const CRT_vector3& n2,
                    const CRT_vector3& uv0, const CRT_vector3& uv1, const CRT_vector3& uv2) const;

    // same as intersect but it just checks if its shadow without the other things
    // refractive materials just dont leave shadows for now
    bool intersect_shadow(const CRT_triangle& T, const CRT_ray& ray, float max_distance) const;

    // closest intersection across the whole scene
    bool intersect_scene(const CRT_ray& ray, CRT_hit& hit_point) const;

    // whether a shadow ray hits anything before max_distance
    bool is_shadow(const CRT_ray& shadow_ray, float max_distance) const;


// shading functions, one per material type
private:

    // shade material with diffuse material
    CRT_vector3 shade_diffuse    (const CRT_hit& hit_point) const;

    // shade material with reflective material
    CRT_vector3 shade_reflective (const CRT_ray& ray, 
                                const CRT_hit& hit_point,
                                const CRT_vector3& background, 
                                int depth) const;

    // shade material with refractive material
    CRT_vector3 shade_refraction (const CRT_ray& ray, 
                                const CRT_hit& hit_point,
                                const CRT_vector3& background, 
                                int depth) const;

    // shade material with constant material
    CRT_vector3 shade_constant   (const CRT_hit& hit_point) const;

    // recursively traces a single ray and returns its color
    CRT_vector3 trace_ray(const CRT_ray& ray, const CRT_vector3& background, int depth = 0) const;
    
    // rendering single bucket
    void render_bucket(const CRT_bucket& bucket, std::vector<CRT_vector3>& framebuffer) const;

    // writes the already rendered buffer to the file
    void write_buffer_to_file(const std::string& output_file, const std::vector<CRT_vector3>& image_buffer) const;


// data
private:
    CRT_scene scene;

    const CRT_settings&                 settings;
          CRT_camera&                   camera;
    const std::vector<CRT_mesh>&        objects;
    const std::vector<CRT_texture>&     textures;
    const std::vector<CRT_material>&    materials;
    const std::vector<CRT_light>&       lights;

    // render-loop constants

    static constexpr float max_color_component  = 255.0f;
    static constexpr float SHADOW_BIAS          = 1e-2f;
    static constexpr float REFRACTION_BIAS      = 1e-2f;
    static constexpr int   MAX_RAY_DEPTH        = 20;
    static constexpr float IOR_AIR              = 1.0f;
};

#endif
