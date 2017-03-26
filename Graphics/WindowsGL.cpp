/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "WindowsGL.h"

#include "wglext.h"
#include <cstring>
#include <string>


#define WGL_GET_PROC_ADDRESS(type, name) type name = (type)wglGetProcAddress(#name)


namespace
{
    PIXELFORMATDESCRIPTOR GetDesiredPixelFormatDescriptor()
    {
        PIXELFORMATDESCRIPTOR desiredPixelFormat;
        std::memset(&desiredPixelFormat, 0, sizeof(desiredPixelFormat));

        desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
        desiredPixelFormat.nVersion = 1;
        desiredPixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
        desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
        desiredPixelFormat.cColorBits = 24;
        desiredPixelFormat.cRedBits = 8;
        desiredPixelFormat.cBlueBits = 8;
        desiredPixelFormat.cGreenBits = 8;
        desiredPixelFormat.cDepthBits = 32;

        return desiredPixelFormat;
    }


    class DummyContext
    {
    public:
        DummyContext()
        {
            WNDCLASSEX wndClass;

            wndClass.cbSize = sizeof(WNDCLASSEX);
            wndClass.style = CS_HREDRAW | CS_VREDRAW;
            wndClass.lpfnWndProc = DefWindowProc;
            wndClass.cbClsExtra = 0;
            wndClass.cbWndExtra = 0;
            wndClass.hInstance = GetModuleHandle(nullptr);
            wndClass.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_APPLICATION));
            wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
            wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wndClass.lpszMenuName = NULL;
            wndClass.lpszClassName = L"dummy";
            wndClass.hIconSm = LoadIcon(wndClass.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

            RegisterClassEx(&wndClass);

            hWnd_ = CreateWindow(
                wndClass.lpszClassName,
                L"Dummy Window",
                WS_OVERLAPPEDWINDOW, 
                CW_USEDEFAULT, 
                CW_USEDEFAULT, 
                CW_USEDEFAULT, 
                CW_USEDEFAULT, 
                nullptr, 
                nullptr, 
                GetModuleHandle(nullptr), 
                nullptr);

            UpdateWindow(hWnd_);

            hDC_ = GetDC(hWnd_);
            hRC_ = windowsgl::CreateFixedFunctionGLContext(hDC_);

            oldDC_ = wglGetCurrentDC();
            oldRC_ = wglGetCurrentContext();

            wglMakeCurrent(hDC_, hRC_);
        }

        ~DummyContext()
        {
            wglMakeCurrent(oldDC_, oldRC_);
            wglDeleteContext(hRC_);
            DestroyWindow(hWnd_);
        }
    
    private:
        HWND hWnd_;
        HDC hDC_;
        HDC oldDC_;
        HGLRC hRC_;
        HGLRC oldRC_;
    };


    std::string GetGLExtensionString(HDC windowDC)
    {
        DummyContext dummyContext;
        WGL_GET_PROC_ADDRESS(PFNWGLGETEXTENSIONSSTRINGARBPROC, wglGetExtensionsStringARB);
        return wglGetExtensionsStringARB(windowDC);
    }

    // wgl* functions need a current context in order for their address to be 
    // retrieved. However, SetPixelFormat() can only be called on a DC once. As
    // a result, we load a fixed function context (See DummyContext implementation)
    // to get the function pointers, and then we use those to create the modern
    // OpenGL context. 
}


namespace windowsgl 
{
    HGLRC CreateFixedFunctionGLContext(HDC windowDC)
    {
        PIXELFORMATDESCRIPTOR desiredPixelFormat = GetDesiredPixelFormatDescriptor();

        int pixelFormatIndex = ChoosePixelFormat(windowDC, &desiredPixelFormat);
        DescribePixelFormat(
            windowDC,
            pixelFormatIndex,
            sizeof(desiredPixelFormat),
            &desiredPixelFormat);
        BOOL isPixelFormatSet = SetPixelFormat(
            windowDC, 
            pixelFormatIndex, 
            &desiredPixelFormat);

        if (!isPixelFormatSet) {
            return NULL;
        }

        return wglCreateContext(windowDC);
    }


    HGLRC CreateModernGLContext(HDC windowDC, int majorVersion, int minorVersion)
    {
        std::string openglExtensions = GetGLExtensionString(windowDC);

        if (openglExtensions.find("WGL_ARB_create_context") == std::string::npos) {
            return NULL;
        }

        DummyContext dummyContext;
        WGL_GET_PROC_ADDRESS(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);
        WGL_GET_PROC_ADDRESS(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
        
        float fAttribs[] = {0, 0};

        int pixAttribs[] = {
            WGL_SUPPORT_OPENGL_ARB, 1,
            WGL_DRAW_TO_WINDOW_ARB, 1,
            WGL_RED_BITS_ARB, 8,
            WGL_GREEN_BITS_ARB, 8,
            WGL_BLUE_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 16,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            0 };

        int contextAttributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
            WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            0 };

        UINT const maxFormatCount = 1;
        UINT pixelFormatCount;
        int pixelFormatIndex;
        BOOL isPixelFormatChosen = wglChoosePixelFormatARB(
            windowDC, 
            pixAttribs, 
            fAttribs, 
            maxFormatCount, 
            &pixelFormatIndex, 
            &pixelFormatCount);

        if (isPixelFormatChosen && pixelFormatCount != 0) {
            // The next 3 lines are there because SetPixelFormat requires a 
            // PIXELFORMATDESCRIPTOR* as its last argument...
            PIXELFORMATDESCRIPTOR dummyPixelFormatDescriptor;
            std::memset(&dummyPixelFormatDescriptor, 0, sizeof(dummyPixelFormatDescriptor));
            DescribePixelFormat(windowDC, pixelFormatIndex, sizeof(dummyPixelFormatDescriptor), &dummyPixelFormatDescriptor);

            // ... And we need to call SetPixelFormat(), even before creating
            // a modern OpenGL context
            SetPixelFormat(windowDC, pixelFormatIndex, &dummyPixelFormatDescriptor);
            return wglCreateContextAttribsARB(windowDC, NULL, contextAttributes);
        }

        return NULL;
    }


    void DestroyModernGLContext(HWND hWnd, HGLRC hRC)
    {
        HDC hDC = GetDC(hWnd);
        ReleaseDC(hWnd, hDC);
        wglDeleteContext(hRC);
    }
}