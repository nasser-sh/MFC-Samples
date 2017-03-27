/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#pragma once
#include "Mesh.h"


namespace graphics {
namespace ffgl {

    class CRenderer
    {
    public:
        void Init();
        void Draw(CMesh const &mesh);
        void ResizeViewport(int width, int height);
    };
} }


