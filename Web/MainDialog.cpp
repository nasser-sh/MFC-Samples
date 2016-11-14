/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "MainDialog.h"

#include "WebApp.h"
#include <cassert>


using namespace web_app;


BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
    ON_BN_CLICKED(IDC_NEW_BROWSER, &CMainDialog::OnNewBrowser)
END_MESSAGE_MAP()



CMainDialog::CMainDialog(CWebApp *pApp, CWnd *pParent)
: CDialog(CMainDialog::IDD, pParent)
, m_pApp(pApp)
{
    assert(m_pApp != nullptr);
}


void CMainDialog::OnNewBrowser()
{
    m_pApp->OnNewBrowser();
}

