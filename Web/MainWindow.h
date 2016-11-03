/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#pragma once
#include <afxext.h>
#include <afxhtml.h>


namespace web_app
{
    class CMainWindow : public CFrameWnd
    {
    public:
        CMainWindow() = default;
        virtual ~CMainWindow() = default;

    private:
        CHtmlView *m_pHtmlView;

    protected:
        afx_msg int OnCreate(LPCREATESTRUCT pCreateStruct);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        DECLARE_MESSAGE_MAP()
    };
}