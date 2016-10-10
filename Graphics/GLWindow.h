/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#pragma once
#include <afxwin.h>

namespace graphics
{
    class CGLWindow : public CFrameWnd
    {
    public:
        CGLWindow() = default;
        virtual ~CGLWindow() = default;

    private:
        HGLRC m_hGLRC;

    protected:
        afx_msg int OnCreate(LPCREATESTRUCT pCreateStruct);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC); 
        afx_msg void OnPaint();
        DECLARE_MESSAGE_MAP()
    };
}
