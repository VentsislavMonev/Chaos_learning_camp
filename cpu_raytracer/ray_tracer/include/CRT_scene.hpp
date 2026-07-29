#ifndef CRT_SCENE_HPP
#define CRT_SCENE_HPP

#include <iostream>
#include <vector>
#include <string>
#include "CRT_camera.hpp"
#include "CRT_mesh.hpp"

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

    const std::vector<CRT_mesh>& get_objects()  const {return objects;}
    const CRT_camera& get_camera()              const {return camera;}
    const CRT_settings& get_settings()          const {return settings;}
        
private:
    std::string file_name;

    std::vector<CRT_mesh> objects;
    CRT_camera camera;
    CRT_settings settings;
};

#endif