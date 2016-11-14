/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#pragma once
#include <afxwin.h>
#include "resource.h"


namespace web_app
{
    class CWebApp;


    class CMainDialog : public CDialog
    {
    public:
        enum { IDD = IDD_MAIN_DIALOG };

        CMainDialog(CWebApp *pApp, CWnd *pParent = nullptr);
        virtual ~CMainDialog() = default;

    private:
        CWebApp *m_pApp;

    protected:
        afx_msg void OnNewBrowser();
        DECLARE_MESSAGE_MAP()
    };
}