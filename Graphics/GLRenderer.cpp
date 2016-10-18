/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "GLRenderer.h"

#include "PlatformGL.h"


namespace graphics {
namespace ffgl {

    void Init()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING); 
        glEnable(GL_LIGHT0);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);

        GLfloat lightPosition[] = { 0.0f, 0.0f, -2.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
    }


    void Draw(CMesh const &mesh)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glVertexPointer(3, GL_FLOAT, 0, &mesh.Vertices().front());
        glNormalPointer(GL_FLOAT, 0, &mesh.Normals().front());
        glDrawElements(
            GL_TRIANGLES, 
            mesh.Indices().size(),
            GL_UNSIGNED_SHORT, 
            &mesh.Indices().front());
    }


    void ResizeViewport(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

} }