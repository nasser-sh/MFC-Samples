/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "SettingsDialog.h"


using namespace graphics;


CSettingsDialog::CSettingsDialog(CWnd *pParent)
: CDialog(CSettingsDialog::IDD, pParent)
{ }


BOOL CSettingsDialog::OnInitDialog()
{
    CComboBox *pComboBox = (CComboBox*)GetDlgItem(IDC_MSAA_COMBOBOX);
    pComboBox->AddString(_T("4x"));
    pComboBox->AddString(_T("2x"));
    pComboBox->AddString(_T("None"));
    return TRUE;
}

BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
END_MESSAGE_MAP()

