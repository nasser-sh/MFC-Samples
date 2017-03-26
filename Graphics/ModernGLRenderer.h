/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#pragma once
#include "PlatformGL.h"
#include "GLFunctions.h"


namespace graphics {
namespace moderngl {

    class CRenderer : public CGLFunctions
    {
    public:
        void Init();
        void Draw();
        void ResizeViewport(int width, int height);
    
    private:
		GLuint Shader(char const *name, GLenum shader_type);
		GLuint ShaderProgram(char const *vertex, char const *fragment);

        GLuint m_vertexArrayObject;
        GLuint m_vertexBufferObject;
        GLuint m_program;
    };
} }

