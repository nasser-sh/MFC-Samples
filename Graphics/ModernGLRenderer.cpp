/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "ModernGLRenderer.h"

#include <fstream>
#include <string>
#include "PlatformGL.h"



namespace
{
    std::string ReadFile(char const *filename)
    {
        std::string str;
        std::ifstream is(filename);
        is.seekg(0, std::ios::end);
        size_t length = is.tellg();
        str.resize(length);
        is.seekg(0, std::ios::beg);
        is.read(&str[0], length);
        return str;
    }
}


namespace graphics {
namespace moderngl {

    void CRenderer::Init()
    {
		LoadGLFunctions();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLfloat vertices[] = {
             0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        glGenVertexArrays(1, &m_vertexArrayObject);
        glBindVertexArray(m_vertexArrayObject);

        glGenBuffers(1, &m_vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        m_program = ShaderProgram("vertex.glsl", "fragment.glsl");
        glUseProgram(m_program);

        GLint positionAttrib = glGetAttribLocation(m_program, "position");
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }


    void CRenderer::Draw()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }


    void CRenderer::ResizeViewport(int width, int height)
    {
        glViewport(0, 0, width, height);
    }


	GLuint CRenderer::Shader(char const *name, GLenum shader_type)
    {
        GLuint shader = glCreateShader(shader_type);
        std::string shaderSource = ReadFile(name);
        char const *shaderSourceCstr = shaderSource.c_str();
        glShaderSource(shader, 1, &shaderSourceCstr, nullptr);
        glCompileShader(shader);
        return shader;
    }


    GLuint CRenderer::ShaderProgram(char const *vertex, char const *fragment)
    {
        GLuint shaders[2];
        shaders[0] = Shader(vertex, GL_VERTEX_SHADER);
        shaders[1] = Shader(fragment, GL_FRAGMENT_SHADER);

        GLuint program = glCreateProgram();

        for (auto const &shader : shaders) {
            glAttachShader(program, shader);
        }

        glLinkProgram(program);

        for (auto const &shader : shaders) {
            glDetachShader(program, shader);
            glDeleteShader(shader);
        }

        return program;
    }

} }


