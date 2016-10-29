/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "SettingsDialog.h"

#include <algorithm>
#include <cassert>
#include "GraphicsApp.h"
#include "Settings.h"


using namespace graphics;


// TODO: Factor comment into separate project.
// In the CBN_SELCHANGE callback, the edit control might not have yet been
// updated to match the newly selected item. Therefore, when getting the 
// currently selected item, CComboBox::GetLBText() should be used. The index
// passed to it may be retrieved from CComboBox::GetCurSel(); 
namespace
{
    CString ToString(int value)
    {
        CString result;
        result.Format(_T("%d"), value);
        return result;
    }

    // TODO: Factor out to a separate utility library/header.
    template <typename T>
    void SetComboBoxEditValue(CComboBox &comboBox, T const &value)
    {
        CString stringValue = ToString(value);
        
        for (int itemIndex = 0; itemIndex < comboBox.GetCount(); ++itemIndex) {
            CString comboBoxText;
            comboBox.GetLBText(itemIndex, comboBoxText);

            if (comboBoxText == stringValue) {
                comboBox.SetCurSel(itemIndex);
                break;
            }
        }
    }


    void SetComboBoxHeight(CComboBox &comboBox, int height)
    {
        // http://www.codeproject.com/Articles/762/Using-the-CComboBox-control
        CRect rect;
        comboBox.GetWindowRect(&rect);
        rect.bottom = rect.top + height * comboBox.GetItemHeight(-1) + rect.Height();
        comboBox.SetWindowPos(nullptr, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
    }
}


CSettingsDialog::CSettingsDialog(CSettings const &settings, CWnd *pParent)
: CDialog(CSettingsDialog::IDD, pParent)
, m_settings(settings)
, m_isOkClicked(false)
{ }


void CSettingsDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_MSAA_COMBOBOX, m_settings.msaaSampleCount);
    DDX_Control(pDX, IDC_MSAA_COMBOBOX, m_msaaComboBox);
}


BOOL CSettingsDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_msaaComboBox.AddString(_T("4"));
    m_msaaComboBox.AddString(_T("2"));
    m_msaaComboBox.AddString(_T("0"));
    
    SetComboBoxHeight(m_msaaComboBox, m_msaaComboBox.GetCount());
    SetComboBoxEditValue(m_msaaComboBox, m_settings.msaaSampleCount);

    return TRUE;
}


void CSettingsDialog::OnOK()
{
    CDialog::OnOK();
    m_isOkClicked = true;
}


CSettings const &CSettingsDialog::Settings() const
{
    assert(m_isOkClicked);
    return m_settings;
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
END_MESSAGE_MAP()

