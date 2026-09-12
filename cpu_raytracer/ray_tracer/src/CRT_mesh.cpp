#include "CRT_mesh.hpp"

CRT_mesh::CRT_mesh(
    int _material_index,
    const std::vector<CRT_vector3> & _vertices,
    const std::vector<CRT_vector3>& _uvs,
    const std::vector<int> & _triangle_indices
)
    : material_index(_material_index), 
    vertices(_vertices), 
    uvs(_uvs),
    triangle_by_vertices_indices(_triangle_indices)
{
    calculate_vertex_normals();
    build_triangles();
}

void CRT_mesh::get_triangle_vertices(size_t tri_idx, CRT_vector3 &v0, CRT_vector3 &v1, CRT_vector3 &v2) const
{
    if (tri_idx >= triangle_by_vertices_indices.size()/VERTICES_IN_TRIANGLE)
        throw std::out_of_range("CRT_mesh::get_triangle_vertices - triangle index out of range");

    v0 = vertices[triangle_by_vertices_indices[tri_idx*3 + 0]];
    v1 = vertices[triangle_by_vertices_indices[tri_idx*3 + 1]];
    v2 = vertices[triangle_by_vertices_indices[tri_idx*3 + 2]];
}

void CRT_mesh::get_triangle_vertex_normals(size_t tri_idx, CRT_vector3 &n0, CRT_vector3 &n1, CRT_vector3 &n2) const
{
    size_t base = tri_idx * VERTICES_IN_TRIANGLE;
    n0 = vertex_normals[triangle_by_vertices_indices[base + 0]];
    n1 = vertex_normals[triangle_by_vertices_indices[base + 1]];
    n2 = vertex_normals[triangle_by_vertices_indices[base + 2]];
}

void CRT_mesh::get_triangle_uvs(size_t tri_idx, CRT_vector3 &uv0, CRT_vector3 &uv1, CRT_vector3 &uv2) const
{
    if (uvs.empty()) 
    { 
        uv0 = uv1 = uv2 = CRT_vector3(0,0,0); 
        return; 
    }
    int i0 = triangle_by_vertices_indices[tri_idx * VERTICES_IN_TRIANGLE + 0];
    int i1 = triangle_by_vertices_indices[tri_idx * VERTICES_IN_TRIANGLE + 1];
    int i2 = triangle_by_vertices_indices[tri_idx * VERTICES_IN_TRIANGLE + 2];
    uv0 = uvs[i0];
    uv1 = uvs[i1];
    uv2 = uvs[i2];
}

void CRT_mesh::calculate_vertex_normals()
{
    vertex_normals.resize(vertices.size());

    // initialize
    for(CRT_vector3& normal : vertex_normals)
        normal = CRT_vector3(0,0,0);

    size_t count = triangle_by_vertices_indices.size();
    for(size_t i = 0; i < count; i += 3)
    {
        unsigned int i0 = triangle_by_vertices_indices[i];
        unsigned int i1 = triangle_by_vertices_indices[i+1];
        unsigned int i2 = triangle_by_vertices_indices[i+2];

        CRT_vector3 v0 = vertices[i0];
        CRT_vector3 v1 = vertices[i1];
        CRT_vector3 v2 = vertices[i2];

        CRT_vector3 edge1 = v1 - v0;
        CRT_vector3 edge2 = v2 - v0;

        CRT_vector3 face_normal = (edge1 ^ edge2);

        vertex_normals[i0] += face_normal;
        vertex_normals[i1] += face_normal;
        vertex_normals[i2] += face_normal;
    }


    for(CRT_vector3& normal : vertex_normals)
    {
        normal.normalize();
    }
}

void CRT_mesh::build_triangles()
{
    size_t count = triangle_by_vertices_indices.size()/VERTICES_IN_TRIANGLE;
    precomputed_triangles.reserve(count);

    for (size_t i = 0; i < count; ++i)
    {
        CRT_vector3 v0, v1, v2;
        get_triangle_vertices(i, v0, v1, v2);
        precomputed_triangles.emplace_back(v0, v1, v2);
    }
}