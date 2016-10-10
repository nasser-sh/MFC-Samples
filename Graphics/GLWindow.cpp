/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "GLWindow.h"
#include "GLRenderer.h"


using namespace graphics;


BEGIN_MESSAGE_MAP(CGLWindow, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_SIZE()
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
    ffgl::Init();
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
    wglMakeCurrent(hDC, m_hGLRC);
    ffgl::Draw();
    SwapBuffers(hDC);
    wglMakeCurrent(0, 0);
}


void CGLWindow::OnSize(UINT nType, int cx, int cy)
{
    HDC hDC = GetDC()->GetSafeHdc();
    wglMakeCurrent(hDC, m_hGLRC);
    ffgl::ResizeViewport(cx, cy);
    wglMakeCurrent(0, 0);
}