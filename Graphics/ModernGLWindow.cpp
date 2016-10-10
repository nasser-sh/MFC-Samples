#include "ModernGLWindow.h"


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

    SwapBuffers(hDC);
    wglMakeCurrent(0, 0);
}


void CModernGLWindow::OnSize(UINT nType, int cx, int cy)
{
    HDC hDC = GetDC()->GetSafeHdc();
    wglMakeCurrent(hDC, m_hGLRC);

    wglMakeCurrent(0, 0);
}