#include "Fibonacci.h"
#include "FibonacciDialog.h"
#include "resource.h"


using namespace fibonacci;


#define WM_USER_THREAD_FINISHED WM_USER + 0x001


namespace
{
    struct CThreadParams
    {
        HWND m_hWnd;
        int m_inputValue;
    };


    UINT ThreadFunc(LPVOID pParam)
    {
        // Get info from thread
        CThreadParams *pThreadParams = (CThreadParams*)pParam;
        HWND hWnd = pThreadParams->m_hWnd;
        int inputValue = pThreadParams->m_inputValue;
        delete pThreadParams;

        // "Portable" part
        int result = Fibonacci(inputValue);

        // Update GUI
        ::PostMessage(hWnd, WM_USER_THREAD_FINISHED, (WPARAM)result, 0);
        return 0;
    }
}


CFibonacciDialog::CFibonacciDialog(CWnd *pParent)
: CDialog(CFibonacciDialog::IDD, pParent)
{ }


BEGIN_MESSAGE_MAP(CFibonacciDialog, CDialog)
    ON_BN_CLICKED(IDC_CALCULATE, &CFibonacciDialog::OnCalculate)
    ON_MESSAGE(WM_USER_THREAD_FINISHED, &CFibonacciDialog::OnThreadFinished)
END_MESSAGE_MAP()


void CFibonacciDialog::OnCalculate()
{
    int value = (int)GetDlgItemInt(IDC_INPUT_VALUE);
    
    if (value <= 0) {
        AfxMessageBox(L"Invalid Value!");
        GetDlgItem(IDC_INPUT_VALUE)->SetFocus();
        return;
    }

    CThreadParams *pThreadParams = new CThreadParams{m_hWnd, value};
    CWinThread *pThread = AfxBeginThread(ThreadFunc, (LPVOID)pThreadParams);
}


LRESULT CFibonacciDialog::OnThreadFinished(WPARAM wParam, LPARAM lParam)
{
    int result = (int)wParam;

    CString str;
    str.Format(L"Results: %d", result);
    AfxMessageBox(str);

    return S_OK;
}



