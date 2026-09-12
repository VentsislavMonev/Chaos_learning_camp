#ifndef CRT_TEXTURE_HPP
#define CRT_TEXTURE_HPP

#include <algorithm>
#include <string>
#include <stdexcept>
#include "CRT_vector3.hpp"
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
    CRT_vector3 sample(const CRT_hit& hit_point) const;

    CRT_vector3 sample_albedo() const noexcept;

    CRT_vector3 sample_edges(const CRT_hit& hit_point) const noexcept;

    CRT_vector3 sample_checkers(const CRT_hit& hit_point) const;
    
    bool load_bitmap();
    CRT_vector3 sample_bitmap(const CRT_hit& hit_point) const;
    

public:
    std::string name;
    CRT_texture_type type;

    // ALBEDO
    CRT_vector3 albedo{1.0f, 1.0f, 1.0f};

    // EDGES
    CRT_vector3 edge_color{0.0f, 0.0f, 0.0f};
    CRT_vector3 inner_color{1.0f, 1.0f, 1.0f};
    float edge_width = 0.0f;

    // CHECKER
    CRT_vector3 color_A{0.0f, 0.0f, 0.0f};
    CRT_vector3 color_B{1.0f, 1.0f, 1.0f};
    float square_size = 0.1f;

    // BITMAP
    std::string file_path;

    int bitmap_width = 0;
    int bitmap_height = 0;
    int bitmap_channels = 0;

    std::vector<unsigned char> bitmap_buffer;
    
};



#endif