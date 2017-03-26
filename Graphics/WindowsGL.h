/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#pragma once

#include <afxwin.h>
#include <gl/GL.h>
#include "glext.h"


namespace windowsgl
{
    void DestroyModernGLContext(HWND hWnd, HGLRC hRC);
    HGLRC CreateModernGLContext(HDC windowhDC, int majorVersion, int minorVersion);
    HGLRC CreateFixedFunctionGLContext(HDC windowDC);
}
