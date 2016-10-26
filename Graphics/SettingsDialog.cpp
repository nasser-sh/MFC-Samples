/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "SettingsDialog.h"

#include <cassert>
#include "GraphicsApp.h"
#include "Settings.h"


using namespace graphics;


CSettingsDialog::CSettingsDialog(CGraphicsApp *pApp, CWnd *pParent)
: CDialog(CSettingsDialog::IDD, pParent)
, m_pApp(pApp)
{ 
    assert(m_pApp != nullptr);
}


BOOL CSettingsDialog::OnInitDialog()
{
    CComboBox *pComboBox = (CComboBox*)GetDlgItem(IDC_MSAA_COMBOBOX);
    pComboBox->AddString(_T("4x"));
    pComboBox->AddString(_T("2x"));
    pComboBox->AddString(_T("None"));
    return TRUE;
}


void CSettingsDialog::OnOK()
{
    UpdateData();

    CSettings settings;
    m_pApp->OnUpdateSettings(settings);
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
END_MESSAGE_MAP()

