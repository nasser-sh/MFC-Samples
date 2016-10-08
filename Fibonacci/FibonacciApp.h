/*   Copyright (c) nasser-sh 2016
 * 
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
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