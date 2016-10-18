/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#pragma once


namespace graphics
{
    class Vec3
    {
    public:
        Vec3();
        Vec3(Vec3 const&) = default;
        Vec3 &operator=(Vec3 const&) = default;

        Vec3(float xInit, float yInit, float zInit);

        float x;
        float y;
        float z;
    };
}