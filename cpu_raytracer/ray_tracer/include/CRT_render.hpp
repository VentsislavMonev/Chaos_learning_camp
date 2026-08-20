#ifndef CRT_RENDERER_HPP
#define CRT_RENDERER_HPP


#include <string>
#include <vector>
#include "CRT_ray.hpp"
#include "CRT_triangle.hpp"
#include "CRT_camera.hpp"
#include "CRT_scene.hpp"
#include "CRT_light.hpp"
#include "CRT_hit.hpp"
#include "CRT_texture.hpp"

class CRT_render
{
public:
    explicit CRT_render(const std::string& scene_file);

    // Runs the full render loop and writes the result to output_file as a PPM image.
    void render(const std::string& output_file);

// data
private:
    CRT_scene scene;

    const CRT_settings&                 settings;
    const CRT_camera&                   camera;
    const std::vector<CRT_mesh>&        objects;
    const std::vector<CRT_texture>&     textures;
    const std::vector<CRT_material>&    materials;
    const std::vector<CRT_light>&       lights;

// intersection functions
private:
    // low level triangle intersection
    bool intersect( const CRT_triangle& T, const CRT_ray& ray, CRT_hit& hit_point,
                    const CRT_vector& n0, const CRT_vector& n1, const CRT_vector& n2,
                    const CRT_vector& uv0, const CRT_vector& uv1, const CRT_vector& uv2) const;

    // same as intersect but it just checks if its shadow without the other things
    // refractive materials just dont leave shadows for now
    bool intersect_shadow(const CRT_triangle& T, const CRT_ray& ray, float max_distance) const;

    // closest intersection across the whole scene
    bool intersect_scene(const CRT_ray& ray, CRT_hit& hit_point) const;

    // whether a shadow ray hits anything before max_distance
    bool is_shadow(const CRT_ray& shadow_ray, float max_distance) const;


// shading routines, one per material type
private:

    // shade material with diffuse material
    CRT_vector shade_diffuse    (const CRT_hit& hit_point) const;

    // shade material with reflective material
    CRT_vector shade_reflective (const CRT_ray& ray, 
                                const CRT_hit& hit_point,
                                const CRT_vector& background, 
                                int depth) const;

    // shade material with refractive material
    CRT_vector shade_refraction (const CRT_ray& ray, 
                                const CRT_hit& hit_point,
                                const CRT_vector& background, 
                                int depth) const;

    // shade material with constant material
    CRT_vector shade_constant   (const CRT_hit& hit_point) const;

private:
    // recursively traces a single ray and returns its color
    CRT_vector trace_ray(const CRT_ray& ray, const CRT_vector& background, int depth = 0) const;

// static constants
private:
    // render-loop constants
    static constexpr float max_color_component = 255.0f;
    static constexpr float SHADOW_BIAS          = 1e-2f;
    static constexpr float REFRACTION_BIAS      = 1e-2f;
    static constexpr int   MAX_RAY_DEPTH        = 20;
    static constexpr float IOR_AIR              = 1.0f;
};

#endif
