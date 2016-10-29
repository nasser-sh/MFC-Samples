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
    class CSettingsDialog : public CDialog
    {
    public:
        enum { IDD = IDD_SETTINGS_DIALOG };

        CSettingsDialog(CSettings const &settings, CWnd *pParent = nullptr);
        virtual ~CSettingsDialog() = default;

        void DoDataExchange(CDataExchange *pDX) override;
        BOOL OnInitDialog() override;
        void OnOK() override;

        CSettings const &Settings() const;

    private:
        CComboBox m_msaaComboBox;
        CSettings m_settings;
        bool m_isOkClicked;

    protected:
        DECLARE_MESSAGE_MAP()
    };
}
