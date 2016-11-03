/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "WebApp.h"

#include "MainWindow.h"


using namespace web_app;


CWebApp theApp;


BOOL CWebApp::InitInstance()
{
    CMainWindow *pWindow = new CMainWindow;
    pWindow->Create(nullptr, _T("Hello, Web!"));

    m_pMainWnd = pWindow;
    m_pMainWnd->UpdateWindow();
    m_pMainWnd->ShowWindow(m_nCmdShow);

    return TRUE;
}