/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "MainDialog.h"
#include "GLWindow.h"
#include "ModernGLWindow.h"
#include "SettingsDialog.h"


using namespace graphics;


BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
    ON_BN_CLICKED(IDC_FIXED_FUNCTION_GL_WINDOW, &CMainDialog::OnFixedFunctionGLWindow)
    ON_BN_CLICKED(IDC_MODERN_GL_WINDOW, &CMainDialog::OnModernGLWindow)
    ON_BN_CLICKED(IDC_SETTINGS, &CMainDialog::OnSettings)
END_MESSAGE_MAP()


CMainDialog::CMainDialog(CWnd *pParent)
: CDialog(CMainDialog::IDD, pParent)
{ }


void CMainDialog::OnFixedFunctionGLWindow()
{
    CGLWindow *pGLWindow = new CGLWindow;
    pGLWindow->Create(nullptr, L"Fixed Function GL");
    pGLWindow->UpdateWindow();
    pGLWindow->ShowWindow(SW_SHOW);
}


void CMainDialog::OnModernGLWindow()
{
    CModernGLWindow *pGLWindow = new CModernGLWindow;
    pGLWindow->Create(nullptr, L"Modern OpenGL");
    pGLWindow->UpdateWindow();
    pGLWindow->ShowWindow(SW_SHOW);
}


void CMainDialog::OnSettings()
{
    CSettingsDialog settingsDialog;
    settingsDialog.DoModal();
}