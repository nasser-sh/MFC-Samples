/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#pragma once
#include <afxwin.h>
#include "resource.h"

namespace graphics
{
    class CMainDialog : public CDialog
    {
    public:
        enum { IDD = IDD_MAIN_DIALOG };

        CMainDialog(CWnd *pParent = nullptr);
        virtual ~CMainDialog() = default;

    protected:
        afx_msg void OnFixedFunctionGLWindow();
        afx_msg void OnModernGLWindow();
        afx_msg void OnSettings();
        DECLARE_MESSAGE_MAP()
    };
}
