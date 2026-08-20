#ifndef CRT_SCENE_HPP
#define CRT_SCENE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "CRT_camera.hpp"
#include "CRT_mesh.hpp"
#include "CRT_light.hpp"
#include "CRT_material.hpp"
#include "CRT_texture.hpp"

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

namespace fs = std::filesystem;


struct CRT_settings
{
    int image_width;
    int image_height;
    CRT_vector background_color;
};


class CRT_scene
{
public:
    CRT_scene(const std::string& scene_file_name);

    
    void parse_scene_file(const std::string& scene_file_name);
    
    const CRT_camera& get_camera()                      const {return camera;}
    const CRT_settings& get_settings()                  const {return settings;}
    const std::vector<CRT_light>& get_lights()          const {return lights;}
    const std::vector<CRT_texture>& get_textures()      const {return textures;}
    const std::vector<CRT_material>& get_materials()    const {return materials;}
    const std::vector<CRT_mesh>& get_objects()          const {return objects;}
    
private:
    void parse_settings     (const rapidjson::Document& document);
    void parse_camera       (const rapidjson::Document& document);
    void parse_lights       (const rapidjson::Document& document);
    void parse_textures     (const rapidjson::Document& document, std::unordered_map<std::string, int>& texture_name_to_index);
    void parse_materials    (const rapidjson::Document& document, const std::unordered_map<std::string, int>& texture_name_to_index);
    void parse_objects      (const rapidjson::Document& document);

private:
    std::string file_name;

    CRT_settings settings;
    CRT_camera camera;
    std::vector<CRT_light> lights;
    std::vector<CRT_texture> textures;
    std::vector<CRT_material> materials;
    std::vector<CRT_mesh> objects;
};

#endif