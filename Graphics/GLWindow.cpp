/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "GLWindow.h"

#include "GLRenderer.h"
#include "MeshLoaders.h"
#include "PlatformGL.h"


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
    
    HDC hDC = GetDC()->GetSafeHdc();
    m_hGLRC = windowsgl::CreateFixedFunctionGLContext(hDC);

    wglMakeCurrent(hDC, m_hGLRC);
    m_renderer.Init();
    wglMakeCurrent(0, 0);

    m_mesh = mesh_loaders::LoadWavefrontObjFile("suzanne.wavefront");
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
    m_renderer.Draw(m_mesh);
    SwapBuffers(hDC);
    wglMakeCurrent(0, 0);
}


void CGLWindow::OnSize(UINT nType, int cx, int cy)
{
    HDC hDC = GetDC()->GetSafeHdc();
    wglMakeCurrent(hDC, m_hGLRC);
    m_renderer.ResizeViewport(cx, cy);
    wglMakeCurrent(0, 0);
}