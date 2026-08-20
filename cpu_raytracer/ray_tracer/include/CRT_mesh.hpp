#ifndef CRT_MESH_HPP
#define CRT_MESH_HPP

#include <iostream>
#include <vector>
#include "CRT_vector.hpp"

static const int VERTICES_IN_TRIANGLE = 3;

class CRT_mesh
{
public:
    CRT_mesh();
    CRT_mesh(
        int material_index,
        const std::vector<CRT_vector>& _vertices,
        const std::vector<int>& _triangle_indices,
        const std::vector<CRT_vector>& _uvs
    );
    ~CRT_mesh()=default;

    int get_material_index()                        const {return material_index;}
    const std::vector<CRT_vector>& get_vertices()   const {return vertices;}
    const std::vector<int>& get_triangle_indices()  const {return triangle_by_vertices_indices;}
    const std::vector<CRT_vector>& get_uvs()        const {return uvs;}
    CRT_vector get_vertex_normal(size_t index)      const {return vertex_normals[index];}
    size_t get_triangle_count()                     const {return triangle_by_vertices_indices.size()/VERTICES_IN_TRIANGLE;}

    void get_triangle_vertices(size_t tri_idx, CRT_vector& v0, CRT_vector& v1, CRT_vector& v2) const;
    void get_triangle_vertex_normals(size_t tri_idx, CRT_vector& n0, CRT_vector& n1, CRT_vector& n2) const;
    void get_triangle_uvs(size_t tri_idx, CRT_vector& uv0, CRT_vector& uv1, CRT_vector& uv2) const;

    void calculate_vertex_normals();

private:
    int material_index;
    std::vector<CRT_vector> vertices;
    std::vector<int> triangle_by_vertices_indices;
    std::vector<CRT_vector> uvs;

    std::vector<CRT_vector> vertex_normals;
};


#endif