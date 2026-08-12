#include "CRT_scene.hpp"

#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

using namespace rapidjson;

CRT_scene::CRT_scene(const std::string &scene_file_name)
{
    file_name=scene_file_name;
    parse_scene_file(scene_file_name);
}

CRT_vector parse_vector3(const Value& arr, const std::string& scene_file_name)
{
    if (!arr.IsArray() || arr.Size() != 3)
        throw std::runtime_error("CRT_scene: Expected a JSON array of 3 numbers for a vector in file: "+ scene_file_name);

    return CRT_vector(
        arr[0].GetFloat(),
        arr[1].GetFloat(),
        arr[2].GetFloat()
    );
}

// Parses a flat JSON array of numbers (grouped in 3s: x,y,z,x,y,z,...) into vertices.
std::vector<CRT_vector> parse_vertices(const Value& arr, const std::string& scene_file_name)
{
    size_t arr_size = arr.Size();
    if (!arr.IsArray() || arr_size % 3 != 0)
        throw std::runtime_error("CRT_scene: Vertices array must contain a multiple of 3 numbers in file: "+ scene_file_name);

    std::vector<CRT_vector> vertices;
    vertices.reserve(arr_size / 3);

    for (SizeType i = 0; i < arr_size; i += 3)
    {
        vertices.emplace_back(
            arr[i].GetFloat(),
            arr[i + 1].GetFloat(),
            arr[i + 2].GetFloat()
        );
    }

    return vertices;
}

// Parses a flat JSON array of vertex indices (grouped in 3s per triangle)
std::vector<int> parse_triangle_indices(const Value& arr, const std::string& scene_file_name)
{
    size_t arr_size = arr.Size();
    if (!arr.IsArray() || arr_size % 3 != 0)
        throw std::runtime_error("CRT_scene: Triangles array must contain a multiple of 3 indices in file: " + scene_file_name);

    std::vector<int> indices;
    indices.reserve(arr_size);

    for (SizeType i = 0; i < arr_size; ++i)
        indices.push_back(arr[i].GetInt());

    return indices;
}

void CRT_scene::parse_scene_file(const std::string &scene_file_name)
{
    std::ifstream file(scene_file_name);
    if(!file) throw std::runtime_error("CRT_scene: Could not open scene file: " + scene_file_name);

    IStreamWrapper isw(file);
    Document document;
    document.ParseStream(isw);

    if(document.HasParseError())
        throw std::runtime_error("CRT_scene: Failed to parse JSON in file: " + scene_file_name);
    

    // settings
    if (document.HasMember("settings") && document["settings"].IsObject())
    {
        const Value& settings_val = document["settings"];
 
        // background color
        if (settings_val.HasMember("background_color"))
            settings.background_color = parse_vector3(settings_val["background_color"], file_name);
 
        // image_settings
        if (settings_val.HasMember("image_settings") && settings_val["image_settings"].IsObject())
        {
            const Value& img_settings = settings_val["image_settings"];
            
            // width
            if (img_settings.HasMember("width"))
                settings.image_width = img_settings["width"].GetInt();
 
            // height
            if (img_settings.HasMember("height"))
                settings.image_height = img_settings["height"].GetInt();
        }
    }

    // camera
    if(document.HasMember("camera")&&document["camera"].IsObject())
    {
        const Value& camera_val = document["camera"];
        CRT_matrix camera_matrix;
        CRT_vector camera_position;

        // matrix 
        if(camera_val.HasMember("matrix")&&camera_val["matrix"].IsArray())
        {
            if (camera_val["matrix"].Size() != 9)
                throw std::runtime_error("CRT_scene: camera matrix must have exactly 9 elements in file: "+ scene_file_name);
            
            camera_matrix = CRT_matrix( camera_val["matrix"][0].GetFloat(), camera_val["matrix"][1].GetFloat(), camera_val["matrix"][2].GetFloat(), 
                                        camera_val["matrix"][3].GetFloat(), camera_val["matrix"][4].GetFloat(), camera_val["matrix"][5].GetFloat(), 
                                        camera_val["matrix"][6].GetFloat(), camera_val["matrix"][7].GetFloat(), camera_val["matrix"][8].GetFloat());
        }
        
        // position 
        if(camera_val.HasMember("position"))
            camera_position = parse_vector3(camera_val["position"], file_name);

        camera = CRT_camera(camera_position, settings.image_width, settings.image_height, camera_matrix);
    }

    // lights
    if(document.HasMember("lights")&&document["lights"].IsArray())
    {
        const Value& lights_val = document["lights"];

        size_t lights_count = lights_val.Size();
        lights.reserve(lights_count);

        // individual lights
        for (size_t light_index = 0; light_index < lights_count; light_index++)
        {
            const Value& light_val = lights_val[light_index];
            CRT_vector light_position;
            int light_intensity = 0;
            
            // position
            if (light_val.HasMember("position"))
            light_position = parse_vector3(light_val["position"], file_name);
            
            // intensity
            if (light_val.HasMember("intensity"))
            light_intensity = light_val["intensity"].GetInt();
            lights.emplace_back(light_position, light_intensity);
        }
    }

    // materials
    if (document.HasMember("materials") && document["materials"].IsArray())
    {
        const Value& materials_val = document["materials"];

        materials.reserve(materials_val.Size());

        // single material
        for (SizeType i = 0; i < materials_val.Size(); ++i)
        {
            const Value& material_val = materials_val[i];

            CRT_material_type type = CRT_material_type::DIFFUSE;
            CRT_vector albedo(1.0f, 1.0f, 1.0f);
            bool smooth_shading = false;

            // type
            if (material_val.HasMember("type") && material_val["type"].IsString())
            {
                std::string type_str = material_val["type"].GetString();

                if (type_str == "diffuse")
                    type = CRT_material_type::DIFFUSE;
                else if (type_str == "reflective")
                    type = CRT_material_type::REFLECTIVE;
                else
                    throw std::runtime_error("CRT_scene: Unknown material type: " + type_str);
            }

            // albedo
            if (material_val.HasMember("albedo"))
                albedo = parse_vector3(material_val["albedo"], scene_file_name);

            // smooth_shading
            if (material_val.HasMember("smooth_shading"))
                smooth_shading = material_val["smooth_shading"].GetBool();

            materials.emplace_back(CRT_material{
                type,
                albedo,
                smooth_shading
            });
        }
    }

    // objects
    if(document.HasMember("objects")&&document["objects"].IsArray())
    {
        const Value& objects_val = document["objects"];

        size_t objects_count = objects_val.Size();
        objects.reserve(objects_count);

        // individual objects
        for (size_t object_index = 0; object_index < objects_count; object_index++)
        {
            const Value& object_val = objects_val[object_index];
 
            std::vector<CRT_vector> vertices;
            std::vector<int> triangle_indices;
            int material_index = 0;
 
            // material_index
            if (object_val.HasMember("material_index"))
                material_index = object_val["material_index"].GetInt();

            // vertices
            if (object_val.HasMember("vertices"))
                vertices = parse_vertices(object_val["vertices"], file_name);
 
            // triangle indices
            if (object_val.HasMember("triangles"))
                triangle_indices = parse_triangle_indices(object_val["triangles"], file_name);
 
            objects.emplace_back(material_index, vertices, triangle_indices);
        }
    }
}
