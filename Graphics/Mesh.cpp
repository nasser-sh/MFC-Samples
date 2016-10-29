/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "Mesh.h"

#include <cassert>
#include <fstream>
#include <string>


using namespace graphics;


CMesh::CMesh(
    std::vector<Vec3> vertices, 
    std::vector<Vec3> normals, 
    std::vector<GLushort> indices)
: m_vertices(std::move(vertices))
, m_normals(std::move(normals))
, m_indices(std::move(indices))
{ }


std::vector<Vec3> const &CMesh::Vertices() const
{
    return m_vertices;
}


std::vector<Vec3> const &CMesh::Normals() const
{
    return m_normals;
}


std::vector<GLushort> const &CMesh::Indices() const
{
    return m_indices;
}