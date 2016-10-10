/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "GLWindow.h"
#include <gl/GL.h>


using namespace graphics;


BEGIN_MESSAGE_MAP(CGLWindow, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()


int CGLWindow::OnCreate(LPCREATESTRUCT pCreateStruct)
{
    if (CFrameWnd::OnCreate(pCreateStruct) == -1) {
        return -1;
    }

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    
        PFD_TYPE_RGBA,            
        32,                        
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                     
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0 };
    
    HDC hDC = GetDC()->GetSafeHdc();
    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    BOOL isPixelFormatSet = SetPixelFormat(hDC, pixelFormat, &pfd);

    m_hGLRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, m_hGLRC);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

    wglMakeCurrent(0, 0);
    return 0;
}


BOOL CGLWindow::OnEraseBkgnd(CDC *pDC)
{
    if (pDC == nullptr) {
        return CFrameWnd::OnEraseBkgnd(pDC);
    }

    return FALSE;
}


void CGLWindow::OnPaint()
{
    HDC hDC = GetDC()->GetSafeHdc();
    CRect clientRect;
    GetClientRect(clientRect);

    wglMakeCurrent(hDC, m_hGLRC);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, clientRect.Width(), clientRect.Height());

    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glEnd();

    SwapBuffers(hDC);
    wglMakeCurrent(0, 0);
}
