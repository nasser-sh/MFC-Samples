/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "MainDialog.h"

#include "GraphicsApp.h"
#include "SettingsDialog.h"
#include <cassert>


using namespace graphics;


BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
    ON_BN_CLICKED(IDC_FIXED_FUNCTION_GL_WINDOW, &CMainDialog::OnFixedFunctionGLWindow)
    ON_BN_CLICKED(IDC_MODERN_GL_WINDOW, &CMainDialog::OnModernGLWindow)
    ON_BN_CLICKED(IDC_SETTINGS, &CMainDialog::OnSettings)
END_MESSAGE_MAP()


CMainDialog::CMainDialog(CGraphicsApp *pApp, CWnd *pParent)
: CDialog(CMainDialog::IDD, pParent)
, m_pApp(pApp)
{ 
    assert(m_pApp != nullptr);
}


void CMainDialog::OnFixedFunctionGLWindow()
{
    m_pApp->OnFixedFunctionGLWindow();
}


void CMainDialog::OnModernGLWindow()
{
    m_pApp->OnModernGLWindow();
}


void CMainDialog::OnSettings()
{
    CSettingsDialog settingsDialog;
    settingsDialog.DoModal();
}