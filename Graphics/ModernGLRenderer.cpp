/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include <afxwin.h>
#include <gl/GL.h>
#include "ModernGLRenderer.h"


namespace graphics {
namespace moderngl {

    void Init()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
    }


    void Draw()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }


    void ResizeViewport(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

} }
