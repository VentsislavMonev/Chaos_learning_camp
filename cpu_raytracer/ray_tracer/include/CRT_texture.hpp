#ifndef CRT_TEXTURE_HPP
#define CRT_TEXTURE_HPP

#include <algorithm>
#include <string>
#include "CRT_vector.hpp"
#include "CRT_hit.hpp"

#include "stb_image.h" 

enum class CRT_texture_type 
{ 
    ALBEDO,
    EDGES,
    CHECKER,
    BITMAP 
};

// TODO: later add polymorphism
class CRT_texture
{
public:
    std::string name;
    CRT_texture_type type;

    // ALBEDO
    CRT_vector albedo{1.0f, 1.0f, 1.0f};

    // EDGES
    CRT_vector edge_color{0.0f, 0.0f, 0.0f};
    CRT_vector inner_color{1.0f, 1.0f, 1.0f};
    float edge_width = 0.0f;

    // CHECKER
    CRT_vector color_A{0.0f, 0.0f, 0.0f};
    CRT_vector color_B{1.0f, 1.0f, 1.0f};
    float square_size = 0.1f;

    // BITMAP
    std::string file_path;

    int bitmap_width = 0;
    int bitmap_height = 0;
    int bitmap_channels = 0;

    std::vector<unsigned char> bitmap_buffer;
    
    
public:
    CRT_vector sample(const CRT_hit& hit_point) const
    {
        switch (type)
        {
            case CRT_texture_type::ALBEDO:
            {
                return sample_albedo();
                break;
            }
            case CRT_texture_type::EDGES:
            {
                return sample_edges(hit_point);
                break;
            }
            case CRT_texture_type::CHECKER:
            {
                return sample_checkers(hit_point);
                break;
            }
            case CRT_texture_type::BITMAP:
            {
                return sample_bitmap(hit_point);
                break;
            }
            default:
                break;
        }
    }

    CRT_vector sample_albedo() const
    {
        return albedo;
    }

    CRT_vector sample_edges(const CRT_hit& hit_point) const
    {
        if(
            hit_point.barycentric.x < edge_width ||
            hit_point.barycentric.y < edge_width ||
            hit_point.barycentric.z < edge_width
        )
        {
            return edge_color;
        }
        else {return inner_color;}
    }

    CRT_vector sample_checkers(const CRT_hit& hit_point) const
    {
        int u_square = static_cast<int>(hit_point.uv.x / square_size);
        int v_square = static_cast<int>(hit_point.uv.y / square_size);
        return ((u_square + v_square) % 2 == 0) ? color_A : color_B;
    }
    
    bool load_bitmap()
    {
        int width;
        int height;
        int channels;

        unsigned char* pixels = stbi_load(
            file_path.c_str(),
            &width,
            &height,
            &channels,
            0
        );

        if (pixels == nullptr)
        {
            return false;
        }

        bitmap_width = width;
        bitmap_height = height;
        bitmap_channels = channels;

        size_t buffer_size =
            static_cast<size_t>(width) *
            static_cast<size_t>(height) *
            static_cast<size_t>(channels);

        bitmap_buffer.assign(pixels, pixels + buffer_size);

        stbi_image_free(pixels);

        return true;
    }

    CRT_vector sample_bitmap(const CRT_hit& hit_point) const
    {        
        // Missing or unloaded texture
        if (
            bitmap_buffer.empty() ||
            bitmap_width <= 0 ||
            bitmap_height <= 0 ||
            bitmap_channels <= 0
        )
        {
            // Purple. Vibrant color to see when something has failed
            return CRT_vector(1.0f, 0.0f, 1.0f);
        }

        // Read UV coordinates

        float u = hit_point.uv.x;
        float v = hit_point.uv.y;

        // stb_image's image origin is top-left, while UVs commonly use bottom-left.
        v = 1.0f - v;

        // image position
        int x = static_cast<int>(u * static_cast<float>(bitmap_width - 1));
        int y = static_cast<int>(v * static_cast<float>(bitmap_height - 1));

        size_t pixel_index = (static_cast<size_t>(y) * bitmap_width + x) * bitmap_channels;

        float r = bitmap_buffer[pixel_index] / 255.0f;

        float g = (bitmap_channels >= 2)
                ? bitmap_buffer[pixel_index + 1] / 255.0f
                : r;

        float b = (bitmap_channels >= 3)
                ? bitmap_buffer[pixel_index + 2] / 255.0f
                : r;

        return CRT_vector(r, g, b);
    }
};



#endif