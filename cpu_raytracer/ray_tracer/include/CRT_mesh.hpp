#ifndef CRT_MESH_HPP
#define CRT_MESH_HPP

#include <iostream>
#include <vector>
#include "CRT_vector.hpp"

static const int VERTICES_IN_TRIANGLE = 3;

class CRT_mesh
{
public:
    CRT_mesh()=default;
    CRT_mesh(const std::vector<CRT_vector>& _vertices, const std::vector<int>& triangle_indices)
        :vertices(_vertices), triangle_by_vertices_indices(triangle_indices) {}
    ~CRT_mesh()=default;

    const std::vector<CRT_vector>& get_vertices()   const {return vertices;}
    const std::vector<int>& get_triangle_indices()  const {return triangle_by_vertices_indices;}

    size_t get_triangle_count() const {return triangle_by_vertices_indices.size()/VERTICES_IN_TRIANGLE;}

    void get_triangle_vertices(size_t tri_idx, CRT_vector& v0, CRT_vector& v1, CRT_vector& v2) const
    {
        if (tri_idx >= get_triangle_count())
            throw std::out_of_range("CRT_mesh::get_triangle_vertices - triangle index out of range");

        v0 = vertices[triangle_by_vertices_indices[tri_idx*3 + 0]];
        v1 = vertices[triangle_by_vertices_indices[tri_idx*3 + 1]];
        v2 = vertices[triangle_by_vertices_indices[tri_idx*3 + 2]];
    }

private:
    std::vector<CRT_vector> vertices;
    std::vector<int> triangle_by_vertices_indices;
};


#endif