/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "WebApp.h"

#include "MainDialog.h"
#include "MainWindow.h"


using namespace web_app;


CWebApp theApp;


BOOL CWebApp::InitInstance()
{
    CMainDialog mainDialog(this);
    mainDialog.DoModal();
    return FALSE;
}


void CWebApp::OnNewBrowser()
{
    CMainWindow *pWindow = new CMainWindow;
    pWindow->Create(nullptr, _T("Hello, Web!"));
    pWindow->UpdateWindow();
    pWindow->ShowWindow(SW_SHOW);
}