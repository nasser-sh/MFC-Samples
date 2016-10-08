/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "FibonacciApp.h"
#include "FibonacciDialog.h"


using namespace fibonacci;


CFibonacciApp theApp;


BOOL CFibonacciApp::InitInstance()
{
    CFibonacciDialog fibonacciDialog;
    int response = fibonacciDialog.DoModal();
    return FALSE; // So that it exits, and not launch the message pump
}