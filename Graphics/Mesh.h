/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#pragma once
#include <vector>
#include "PlatformGL.h"
#include "Vec3.h"


namespace graphics
{
    class CMesh
    {
    public:
        CMesh() = default;
        CMesh(CMesh const&) = default;
        CMesh(CMesh&&) = default;
        CMesh &operator=(CMesh const&) = default;
        CMesh &operator=(CMesh&&) = default;

        CMesh(
            std::vector<Vec3> vertices, 
            std::vector<Vec3> normals, 
            std::vector<GLushort> indices);

        std::vector<Vec3> const &Vertices() const;
        std::vector<Vec3> const &Normals() const;
        std::vector<GLushort> const &Indices() const;

    private:
        std::vector<Vec3> m_vertices;
        std::vector<Vec3> m_normals;
        std::vector<GLushort> m_indices;
    };
}

