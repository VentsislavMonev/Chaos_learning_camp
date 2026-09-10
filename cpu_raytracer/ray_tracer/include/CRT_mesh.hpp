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
        const std::vector<CRT_vector3>& _vertices,
        const std::vector<int>& _triangle_indices,
        const std::vector<CRT_vector3>& _uvs
    );
    ~CRT_mesh()=default;

    int get_material_index()                        const {return material_index;}
    const std::vector<CRT_vector3>& get_vertices()   const {return vertices;}
    const std::vector<int>& get_triangle_indices()  const {return triangle_by_vertices_indices;}
    const std::vector<CRT_vector3>& get_uvs()        const {return uvs;}
    CRT_vector3 get_vertex_normal(size_t index)      const {return vertex_normals[index];}
    size_t get_triangle_count()                     const {return triangle_by_vertices_indices.size()/VERTICES_IN_TRIANGLE;}

    void get_triangle_vertices(size_t tri_idx, CRT_vector3& v0, CRT_vector3& v1, CRT_vector3& v2) const;
    void get_triangle_vertex_normals(size_t tri_idx, CRT_vector3& n0, CRT_vector3& n1, CRT_vector3& n2) const;
    void get_triangle_uvs(size_t tri_idx, CRT_vector3& uv0, CRT_vector3& uv1, CRT_vector3& uv2) const;

    void calculate_vertex_normals();

private:
    int material_index;
    std::vector<CRT_vector3> vertices;
    std::vector<int> triangle_by_vertices_indices;
    std::vector<CRT_vector3> uvs;

    std::vector<CRT_vector3> vertex_normals;
};


#endif