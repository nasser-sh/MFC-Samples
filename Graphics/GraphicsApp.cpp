/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#include "GraphicsApp.h"

#include "MainDialog.h"


using namespace graphics;


CGraphicsApp theApp;


BOOL CGraphicsApp::InitInstance()
{
    CMainDialog mainDialog;
    mainDialog.DoModal();
    return FALSE;
}