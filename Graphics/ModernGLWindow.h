#pragma once
#include <afxwin.h>


namespace graphics
{
    class CModernGLWindow : public CFrameWnd
    {
    public:
        CModernGLWindow() = default;
        virtual ~CModernGLWindow() = default;

    private:
        HGLRC m_hGLRC;

    protected:
        afx_msg int OnCreate(LPCREATESTRUCT pCreateStruct);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        DECLARE_MESSAGE_MAP()
    };
}

