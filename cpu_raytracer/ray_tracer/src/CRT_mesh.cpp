#include "CRT_mesh.hpp"

CRT_mesh::CRT_mesh()
{
    calculate_vertex_normals();
}

CRT_mesh::CRT_mesh(int _material_index, const std::vector<CRT_vector> &_vertices, const std::vector<int> &triangle_indices)
    : material_index(_material_index), vertices(_vertices), triangle_by_vertices_indices(triangle_indices)
{
    calculate_vertex_normals();
}

void CRT_mesh::get_triangle_vertices(size_t tri_idx, CRT_vector &v0, CRT_vector &v1, CRT_vector &v2) const
{
    if (tri_idx >= get_triangle_count())
        throw std::out_of_range("CRT_mesh::get_triangle_vertices - triangle index out of range");

    v0 = vertices[triangle_by_vertices_indices[tri_idx*3 + 0]];
    v1 = vertices[triangle_by_vertices_indices[tri_idx*3 + 1]];
    v2 = vertices[triangle_by_vertices_indices[tri_idx*3 + 2]];
}

void CRT_mesh::get_triangle_vertex_normals(size_t tri_idx, CRT_vector &n0, CRT_vector &n1, CRT_vector &n2) const
{
    size_t base = tri_idx * VERTICES_IN_TRIANGLE;
    n0 = vertex_normals[triangle_by_vertices_indices[base + 0]];
    n1 = vertex_normals[triangle_by_vertices_indices[base + 1]];
    n2 = vertex_normals[triangle_by_vertices_indices[base + 2]];
}

void CRT_mesh::calculate_vertex_normals()
{
    vertex_normals.resize(vertices.size());

    // initialize
    for(CRT_vector& normal : vertex_normals)
        normal = CRT_vector(0,0,0);

    size_t count = triangle_by_vertices_indices.size();
    for(size_t i = 0; i < count; i += 3)
    {
        unsigned int i0 = triangle_by_vertices_indices[i];
        unsigned int i1 = triangle_by_vertices_indices[i+1];
        unsigned int i2 = triangle_by_vertices_indices[i+2];

        CRT_vector v0 = vertices[i0];
        CRT_vector v1 = vertices[i1];
        CRT_vector v2 = vertices[i2];

        CRT_vector edge1 = v1 - v0;
        CRT_vector edge2 = v2 - v0;

        CRT_vector face_normal = (edge1 ^ edge2.normalize());

        vertex_normals[i0] += face_normal;
        vertex_normals[i1] += face_normal;
        vertex_normals[i2] += face_normal;
    }


    for(CRT_vector& normal : vertex_normals)
    {
        normal.normalize();
    }
}