/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "GraphicsApp.h"

#include "GLWindow.h"
#include "MainDialog.h"
#include "ModernGLWindow.h"


using namespace graphics;


CGraphicsApp theApp;


BOOL CGraphicsApp::InitInstance()
{
    CMainDialog mainDialog(this);
    mainDialog.DoModal();
    return FALSE;
}


void CGraphicsApp::OnFixedFunctionGLWindow()
{
    CGLWindow *pGLWindow = new CGLWindow;
    pGLWindow->Create(nullptr, L"Fixed Function GL");
    pGLWindow->UpdateWindow();
    pGLWindow->ShowWindow(SW_SHOW);
}


void CGraphicsApp::OnModernGLWindow()
{
    CModernGLWindow *pGLWindow = new CModernGLWindow;
    pGLWindow->Create(nullptr, L"Fixed Function GL");
    pGLWindow->UpdateWindow();
    pGLWindow->ShowWindow(SW_SHOW);
}