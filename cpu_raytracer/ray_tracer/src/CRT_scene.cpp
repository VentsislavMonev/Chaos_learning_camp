#include "CRT_scene.hpp"

#include <fstream>
#include <string>

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


void CRT_scene::parse_settings(const Document &document)
{
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
}

void CRT_scene::parse_camera(const Document &document)
{
    if(document.HasMember("camera")&&document["camera"].IsObject())
    {
        const Value& camera_val = document["camera"];
        CRT_matrix camera_matrix;
        CRT_vector camera_position;

        // matrix 
        if(camera_val.HasMember("matrix")&&camera_val["matrix"].IsArray())
        {
            if (camera_val["matrix"].Size() != 9)
                throw std::runtime_error("CRT_scene: camera matrix must have exactly 9 elements in file: "+ file_name);
            
            camera_matrix = CRT_matrix( camera_val["matrix"][0].GetFloat(), camera_val["matrix"][1].GetFloat(), camera_val["matrix"][2].GetFloat(), 
                                        camera_val["matrix"][3].GetFloat(), camera_val["matrix"][4].GetFloat(), camera_val["matrix"][5].GetFloat(), 
                                        camera_val["matrix"][6].GetFloat(), camera_val["matrix"][7].GetFloat(), camera_val["matrix"][8].GetFloat());
        }
        
        // position 
        if(camera_val.HasMember("position"))
            camera_position = parse_vector3(camera_val["position"], file_name);

        camera = CRT_camera(camera_position, settings.image_width, settings.image_height, camera_matrix);
    }
}

void CRT_scene::parse_lights(const Document &document)
{
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
}

void CRT_scene::parse_textures(const Document &document, std::unordered_map<std::string, int> &texture_name_to_index)
{
    fs::path scene_dir = fs::path(file_name).parent_path();

    if (document.HasMember("textures") && document["textures"].IsArray())
    {
        const Value& textures_val = document["textures"];
        SizeType count = textures_val.Size();
        textures.reserve(count);
 
        for (SizeType i = 0; i < count; ++i)
        {
            const Value& tex_val = textures_val[i];
            CRT_texture tex;

            if (tex_val.HasMember("name"))
                tex.name = tex_val["name"].GetString();

            std::string type_str = tex_val["type"].GetString();

            if (type_str == "albedo")
            {
                tex.type = CRT_texture_type::ALBEDO;
                if (tex_val.HasMember("albedo"))
                    tex.albedo = parse_vector3(tex_val["albedo"], file_name);
            }
            else if (type_str == "edges")
            {
                tex.type        = CRT_texture_type::EDGES;
                tex.edge_color  = parse_vector3(tex_val["edge_color"], file_name);
                tex.inner_color = parse_vector3(tex_val["inner_color"], file_name);
                tex.edge_width  = tex_val["edge_width"].GetFloat();
            }
            else if (type_str == "checker")
            {
                tex.type        = CRT_texture_type::CHECKER;
                tex.color_A     = parse_vector3(tex_val["color_A"], file_name);
                tex.color_B     = parse_vector3(tex_val["color_B"], file_name);
                tex.square_size = tex_val["square_size"].GetFloat();
            }
            else if (type_str == "bitmap")
            {
                tex.type                    = CRT_texture_type::BITMAP;
                std::string relative_path   = tex_val["file_path"].GetString();
                tex.file_path               = scene_dir.string() + relative_path;
                tex.load_bitmap();
            }
            else
            {
                throw std::runtime_error("CRT_scene: Unknown texture type: " + type_str);
            }

            texture_name_to_index[tex.name] = static_cast<int>(textures.size());
            textures.push_back(tex);
        }
    }
}

void CRT_scene::parse_materials(const Document &document, const std::unordered_map<std::string, int> &texture_name_to_index)
{
    if (document.HasMember("materials") && document["materials"].IsArray())
    {
        const Value& materials_val = document["materials"];

        materials.reserve(materials_val.Size());

        // single material
        for (SizeType i = 0; i < materials_val.Size(); ++i)
        {
            const Value& material_val = materials_val[i];

            CRT_material_type type = CRT_material_type::DIFFUSE;
            int texture_index = -1;
            float ior = 0.0f;
            bool smooth_shading = false;

            // type
            if (material_val.HasMember("type") && material_val["type"].IsString())
            {
                std::string type_str = material_val["type"].GetString();

                if (type_str == "diffuse")
                    type = CRT_material_type::DIFFUSE;
                else if (type_str == "reflective")
                    type = CRT_material_type::REFLECTIVE;
                else if (type_str == "refractive")
                    type = CRT_material_type::REFRACTIVE;
                else if (type_str == "constant")
                    type = CRT_material_type::CONSTANT;
                else
                    throw std::runtime_error("CRT_scene: Unknown material type: " + type_str);
            }
            
            // albedo
            if (material_val.HasMember("albedo") && material_val["albedo"].IsString())
            {
                std::string tex_name = material_val["albedo"].GetString();
                auto it = texture_name_to_index.find(tex_name);
                if (it == texture_name_to_index.end())
                    throw std::runtime_error("CRT_scene: Material references unknown texture: " + tex_name);
                texture_index = it->second;
            }

            // ior
            if (material_val.HasMember("ior"))
                ior = material_val["ior"].GetFloat();

            // smooth_shading
            if (material_val.HasMember("smooth_shading"))
                smooth_shading = material_val["smooth_shading"].GetBool();

            materials.emplace_back(CRT_material{
                type,
                texture_index,
                ior,
                smooth_shading
            });
        }
    }
}

void CRT_scene::parse_objects(const Document &document)
{
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
            std::vector<CRT_vector> uvs;
            std::vector<int> triangle_indices;
            int material_index = 0;
 
            // material_index
            if (object_val.HasMember("material_index"))
                material_index = object_val["material_index"].GetInt();

            // vertices
            if (object_val.HasMember("vertices"))
                vertices = parse_vertices(object_val["vertices"], file_name);

            // uvs
            if (object_val.HasMember("uvs"))
                uvs = parse_vertices(object_val["uvs"], file_name);
 
            // triangle indices
            if (object_val.HasMember("triangles"))
                triangle_indices = parse_triangle_indices(object_val["triangles"], file_name);
 
            objects.emplace_back(material_index, vertices, triangle_indices, uvs);
        }
    }
}

void CRT_scene::parse_scene_file(const std::string &scene_file_name)
{
    file_name = scene_file_name;
    
    std::ifstream file(file_name);
    if(!file) throw std::runtime_error("CRT_scene: Could not open scene file: " + file_name);

    IStreamWrapper isw(file);
    Document document;
    document.ParseStream(isw);

    if(document.HasParseError())
        throw std::runtime_error("CRT_scene: Failed to parse JSON in file: " + file_name);
    

    // settings
    parse_settings(document);

    // camera
    parse_camera(document);

    // lights
    parse_lights(document);

    // textures
    std::unordered_map<std::string, int> texture_name_to_index;
    parse_textures(document, texture_name_to_index);    

    // materials
    parse_materials(document, texture_name_to_index);

    // objects
    parse_objects(document);
}