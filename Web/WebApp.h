/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
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

        void OnNewBrowser();
    };
}