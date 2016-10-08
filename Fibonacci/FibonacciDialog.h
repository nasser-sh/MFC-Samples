#pragma once
#include <afxwin.h>
#include "resource.h"


namespace fibonacci
{
    class CFibonacciDialog : public CDialog
    {
    public:
        enum { IDD = IDD_FIBONACCI_DIALOG };

        CFibonacciDialog(CWnd *pParent = nullptr);
        virtual ~CFibonacciDialog() = default;

    protected:
        afx_msg void OnCalculate();
        afx_msg LRESULT OnThreadFinished(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };
}

