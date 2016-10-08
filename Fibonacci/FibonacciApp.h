#pragma once
#include <afxwin.h>


namespace fibonacci
{
    class CFibonacciApp : public CWinApp
    {
    public:
        CFibonacciApp() = default;
        virtual ~CFibonacciApp() = default;

        BOOL InitInstance() override;
    };
}