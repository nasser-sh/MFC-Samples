#pragma once
#include <afxwin.h>


namespace web_app
{
    class CWebApp : public CWinApp
    {
    public:
        CWebApp() = default;
        virtual ~CWebApp() = default;

        BOOL InitInstance() override;
    };
}