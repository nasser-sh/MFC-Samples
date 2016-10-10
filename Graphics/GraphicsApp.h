/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#pragma once
#include <afxwin.h>

namespace graphics
{
    class CGraphicsApp : public CWinApp
    {
    public:
        CGraphicsApp() = default;
        virtual ~CGraphicsApp() = default;

        BOOL InitInstance() override;
    };
}

