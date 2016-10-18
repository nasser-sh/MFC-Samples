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


/* This isn't a Wavefront Obj standard-compliant implementation. It makes a few assumptions:
 * 1. The data is presented in the following order:
 *     * vertices ("v " line beginning)
 *     * normals  ("vn " line beginning)
 *     * faces    ("f " line beginning)
 * 2. Each vertex index corresponds to a single normal index. This would be the
 *    case if Smooth Shading was used in exporting the OBJ file.
 */
CMesh CMesh::FromWavefrontObjFile(std::string const &filename)
{
    std::ifstream inputStream(filename);
    std::string line;

    while (std::getline(inputStream, line) && line[0] != 'v');
    assert(!line.empty());

    std::vector<Vec3> vertices;

    do {
        char const *token = line.c_str() + 2;
        float x, y, z;
        sscanf_s(token, "%f %f %f", &x, &y, &z);
        vertices.emplace_back(x, y, z);
    } while (std::getline(inputStream, line) && line[0] == 'v' && line[1] == ' ');

    std::vector<Vec3> faceNormals;

    do {
        char const *token = line.c_str() + 3;
        float x, y, z;
        sscanf_s(token, "%f %f %f", &x, &y, &z);
        faceNormals.emplace_back(x, y, z);
    } while (std::getline(inputStream, line) && line[0] == 'v' && line[1] == 'n');

    std::vector<GLushort> normalIndices(faceNormals.size(), 0);
    std::vector<GLushort> indices;

    do {
        char const *token = line.c_str() + 2;
        GLushort v1, v2, v3, n1, n2, n3;
        sscanf_s(token, "%hu//%hu %hu//%hu %hu//%hu", &v1, &n1, &v2, &n2, &v3, &n3);
        indices.push_back(v1 - 1);
        indices.push_back(v2 - 1);
        indices.push_back(v3 - 1);

        normalIndices[n1 - 1] = v1 - 1;
        normalIndices[n2 - 1] = v2 - 1;
        normalIndices[n3 - 1] = v3 - 1;
    } while (std::getline(inputStream, line) && line[0] == 'f' && line[1] == ' ');

    std::vector<Vec3> normals(faceNormals.size());

    for (size_t normalIndex = 0; normalIndex < normalIndices.size(); ++normalIndex) {
        GLushort vboIndex = normalIndices[normalIndex];
        normals[vboIndex] = faceNormals[normalIndex];
    }

    return { std::move(vertices), std::move(normals), std::move(indices) };
}


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