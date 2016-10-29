/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#pragma once
#include <afxwin.h>
#include "resource.h"
#include "Settings.h"


namespace graphics
{
    class CGraphicsApp;


    class CSettingsDialog : public CDialog
    {
    public:
        enum { IDD = IDD_SETTINGS_DIALOG };

        CSettingsDialog(CGraphicsApp *pApp, CWnd *pParent = nullptr);
        virtual ~CSettingsDialog() = default;

        void DoDataExchange(CDataExchange *pDX) override;
        BOOL OnInitDialog() override;
        void OnOK() override;

    private:
        CGraphicsApp *m_pApp;
        CComboBox m_msaaComboBox;
        CSettings m_settings;

    protected:
        DECLARE_MESSAGE_MAP()
    };
}
