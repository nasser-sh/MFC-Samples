#include "MainWindow.h"

#include <cassert>


using namespace web_app;


BEGIN_MESSAGE_MAP(CMainWindow, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()


int CMainWindow::OnCreate(LPCREATESTRUCT pCreateStruct)
{
    if (CFrameWnd::OnCreate(pCreateStruct) == -1) {
        return -1;
    }

    m_pHtmlView = (CHtmlView*)RUNTIME_CLASS(CHtmlView)->CreateObject();
    
    BOOL isCreated = m_pHtmlView->Create(
        nullptr,
        nullptr,
        WS_CHILD | WS_VISIBLE,
        CRect(),
        this,
        AFX_IDW_PANE_FIRST);
    assert(isCreated);

    TCHAR currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);

    CString htmlPath;
    htmlPath.Format(_T("file:///%s/html/index.html"), currentDirectory);

    m_pHtmlView->Navigate2(htmlPath);

    return 0;
}


void CMainWindow::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);

    m_pHtmlView->SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER);
}



