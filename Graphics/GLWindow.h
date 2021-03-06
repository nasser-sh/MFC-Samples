#pragma once
/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include <afxwin.h>
#include "GLRenderer.h"
#include "Mesh.h"


namespace graphics
{
    class CGLWindow : public CFrameWnd
    {
    public:
        CGLWindow() = default;
        virtual ~CGLWindow() = default;

    private:
        HGLRC m_hGLRC;
        ffgl::CRenderer m_renderer;
        CMesh m_mesh;

    protected:
        afx_msg int OnCreate(LPCREATESTRUCT pCreateStruct);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC); 
        afx_msg void OnPaint();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        DECLARE_MESSAGE_MAP()
    };
}
