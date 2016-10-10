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
    class CSettingsDialog : public CDialog
    {
    public:
        enum { IDD = IDD_SETTINGS_DIALOG };

        CSettingsDialog(CWnd *pParent = nullptr);
        virtual ~CSettingsDialog() = default;

        BOOL OnInitDialog() override;

    private:
        CComboBox *m_pMsaaComboBox;

    protected:
        DECLARE_MESSAGE_MAP()
    };
}
