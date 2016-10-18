/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#pragma once
#include "PlatformGL.h"


namespace graphics {
namespace moderngl {

    class CRenderer
    {
    public:
        void Init();
        void Draw();
        void ResizeViewport(int width, int height);
    
    private:
        GLuint m_vertexArrayObject;
        GLuint m_vertexBufferObject;
        GLuint m_program;
    };
} }

