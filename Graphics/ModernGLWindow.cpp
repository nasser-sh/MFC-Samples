/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "ModernGLWindow.h"

#include "ModernGLRenderer.h"
#include "PlatformGL.h"


using namespace graphics;


BEGIN_MESSAGE_MAP(CModernGLWindow, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_SIZE()
END_MESSAGE_MAP()


int CModernGLWindow::OnCreate(LPCREATESTRUCT pCreateStruct)
{
    if (CFrameWnd::OnCreate(pCreateStruct) == -1) {
        return -1;
    }

    HDC hDC = GetDC()->GetSafeHdc();
    m_hGLRC = windowsgl::CreateModernGLContext(hDC, 3, 3);

    wglMakeCurrent(hDC, m_hGLRC);
    windowsgl::LoadGLExtensions();
    m_renderer.Init();
    wglMakeCurrent(0, 0);
    return 0;
}


BOOL CModernGLWindow::OnEraseBkgnd(CDC *pDC)
{
    if (pDC == nullptr) {
        return CFrameWnd::OnEraseBkgnd(pDC);
    }

    return FALSE;
}


void CModernGLWindow::OnPaint()
{
    HDC hDC = GetDC()->GetSafeHdc();
    wglMakeCurrent(hDC, m_hGLRC);
    m_renderer.Draw();
    SwapBuffers(hDC);
    wglMakeCurrent(0, 0);
}


void CModernGLWindow::OnSize(UINT nType, int cx, int cy)
{
    HDC hDC = GetDC()->GetSafeHdc();
    wglMakeCurrent(hDC, m_hGLRC);
    m_renderer.ResizeViewport(cx, cy);
    wglMakeCurrent(0, 0);
}