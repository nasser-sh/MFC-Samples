/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#include "WindowsGL.h"
#include "wglext.h"


PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;


namespace
{
    void LoadGLExtensions();


    PIXELFORMATDESCRIPTOR GetPFD()
    {
        PIXELFORMATDESCRIPTOR pfd;
        ZeroMemory(&pfd, sizeof(pfd));

        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cRedBits = 8;
        pfd.cBlueBits = 8;
        pfd.cGreenBits = 8;
        pfd.cDepthBits = 32;

        return pfd;
    }


    HGLRC LoadCoreGLContext(HDC hDC, int majorVersion, int minorVersion)
    {
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

        int contextAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
            WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            0 };

        PIXELFORMATDESCRIPTOR pfd = GetPFD();
        int compatibilityFormat = ChoosePixelFormat(hDC, &pfd);

        UINT const nMaxFormats = 1;
        UINT numFormats;
        int pixelFormat[nMaxFormats];
        BOOL result = wglChoosePixelFormatARB(hDC, pixAttribs, fAttribs, nMaxFormats, pixelFormat, &numFormats);
        
        if (result && numFormats >= 1) {
            SetPixelFormat(hDC, pixelFormat[0], &pfd);
            return wglCreateContextAttribsARB(hDC, nullptr, contextAttribs);
        } else {
            SetPixelFormat(hDC, compatibilityFormat, &pfd);
            return wglCreateContext(hDC);
        }
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
            hRC_ = windowsgl::CreateFixedFunctionGLContext(hDC_, GetPFD());
            wglMakeCurrent(hDC_, hRC_);
        }

        ~DummyContext()
        {
            wglMakeCurrent(0, 0);
            wglDeleteContext(hRC_);
            DestroyWindow(hWnd_);
        }
    
    private:
        HWND hWnd_;
        HDC hDC_;
        HGLRC hRC_;
    };


    // https://www.opengl.org/wiki/Load_OpenGL_Functions
    void *GetAnyGLFuncAddress(const char *name)
    {
        void *p = (void *)wglGetProcAddress(name);

        if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
            HMODULE module = LoadLibraryA("opengl32.dll");
            p = (void *)GetProcAddress(module, name);
        }

        return p;
    }


    // wgl* functions need a current context in order for their address to be 
    // retrieved. However, SetPixelFormat() can only be called on a DC once. As
    // a result, we load a fixed function context (See DummyContext implementation)
    // to get the function pointers, and then we use those to create the modern
    // OpenGL context. 
    void LoadWGLExtensions()
    {
        DummyContext dummyContext;
        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    }
}


namespace windowsgl 
{
    HGLRC CreateFixedFunctionGLContext(HDC hDC, const PIXELFORMATDESCRIPTOR &pfd)
    {
        int pixelFormat = ChoosePixelFormat(hDC, &pfd);
        SetPixelFormat(hDC, pixelFormat, &pfd);
        return wglCreateContext(hDC);
    }


    HGLRC CreateModernGLContext(HDC hDC, int majorVersion, int minorVersion)
    {
        LoadWGLExtensions();
        HGLRC hRC = LoadCoreGLContext(hDC, majorVersion, minorVersion);
        wglMakeCurrent(hDC, hRC);
        LoadGLExtensions();
        wglMakeCurrent(NULL, NULL);
        return hRC;
    }


    void DestroyModernGLContext(HWND hWnd, HGLRC hRC)
    {
        HDC hDC = GetDC(hWnd);
        ReleaseDC(hWnd, hDC);
        wglDeleteContext(hRC);
    }
}


// GL1.2
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
PFNGLTEXIMAGE3DPROC glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;


// GL1.3
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLSAMPLECOVERAGEPROC glSampleCoverage;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
PFNGLMULTITEXCOORD1DPROC glMultiTexCoord1d;
PFNGLMULTITEXCOORD1DVPROC glMultiTexCoord1dv;
PFNGLMULTITEXCOORD1FPROC glMultiTexCoord1f;
PFNGLMULTITEXCOORD1FVPROC glMultiTexCoord1fv;
PFNGLMULTITEXCOORD1IPROC glMultiTexCoord1i;
PFNGLMULTITEXCOORD1IVPROC glMultiTexCoord1iv;
PFNGLMULTITEXCOORD1SPROC glMultiTexCoord1s;
PFNGLMULTITEXCOORD1SVPROC glMultiTexCoord1sv;
PFNGLMULTITEXCOORD2DPROC glMultiTexCoord2d;
PFNGLMULTITEXCOORD2DVPROC glMultiTexCoord2dv;
PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f;
PFNGLMULTITEXCOORD2FVPROC glMultiTexCoord2fv;
PFNGLMULTITEXCOORD2IPROC glMultiTexCoord2i;
PFNGLMULTITEXCOORD2IVPROC glMultiTexCoord2iv;
PFNGLMULTITEXCOORD2SPROC glMultiTexCoord2s;
PFNGLMULTITEXCOORD2SVPROC glMultiTexCoord2sv;
PFNGLMULTITEXCOORD3DPROC glMultiTexCoord3d;
PFNGLMULTITEXCOORD3DVPROC glMultiTexCoord3dv;
PFNGLMULTITEXCOORD3FPROC glMultiTexCoord3f;
PFNGLMULTITEXCOORD3FVPROC glMultiTexCoord3fv;
PFNGLMULTITEXCOORD3IPROC glMultiTexCoord3i;
PFNGLMULTITEXCOORD3IVPROC glMultiTexCoord3iv;
PFNGLMULTITEXCOORD3SPROC glMultiTexCoord3s;
PFNGLMULTITEXCOORD3SVPROC glMultiTexCoord3sv;
PFNGLMULTITEXCOORD4DPROC glMultiTexCoord4d;
PFNGLMULTITEXCOORD4DVPROC glMultiTexCoord4dv;
PFNGLMULTITEXCOORD4FPROC glMultiTexCoord4f;
PFNGLMULTITEXCOORD4FVPROC glMultiTexCoord4fv;
PFNGLMULTITEXCOORD4IPROC glMultiTexCoord4i;
PFNGLMULTITEXCOORD4IVPROC glMultiTexCoord4iv;
PFNGLMULTITEXCOORD4SPROC glMultiTexCoord4s;
PFNGLMULTITEXCOORD4SVPROC glMultiTexCoord4sv;
PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf;
PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixd;
PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixf;
PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixd;


// GL1.4
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
PFNGLPOINTPARAMETERFPROC glPointParameterf;
PFNGLPOINTPARAMETERFVPROC glPointParameterfv;
PFNGLPOINTPARAMETERIPROC glPointParameteri;
PFNGLPOINTPARAMETERIVPROC glPointParameteriv;
PFNGLFOGCOORDFPROC glFogCoordf;
PFNGLFOGCOORDFVPROC glFogCoordfv;
PFNGLFOGCOORDDPROC glFogCoordd;
PFNGLFOGCOORDDVPROC glFogCoorddv;
PFNGLFOGCOORDPOINTERPROC glFogCoordPointer;
PFNGLSECONDARYCOLOR3BPROC glSecondaryColor3b;
PFNGLSECONDARYCOLOR3BVPROC glSecondaryColor3bv;
PFNGLSECONDARYCOLOR3DPROC glSecondaryColor3d;
PFNGLSECONDARYCOLOR3DVPROC glSecondaryColor3dv;
PFNGLSECONDARYCOLOR3FPROC glSecondaryColor3f;
PFNGLSECONDARYCOLOR3FVPROC glSecondaryColor3fv;
PFNGLSECONDARYCOLOR3IPROC glSecondaryColor3i;
PFNGLSECONDARYCOLOR3IVPROC glSecondaryColor3iv;
PFNGLSECONDARYCOLOR3SPROC glSecondaryColor3s;
PFNGLSECONDARYCOLOR3SVPROC glSecondaryColor3sv;
PFNGLSECONDARYCOLOR3UBPROC glSecondaryColor3ub;
PFNGLSECONDARYCOLOR3UBVPROC glSecondaryColor3ubv;
PFNGLSECONDARYCOLOR3UIPROC glSecondaryColor3ui;
PFNGLSECONDARYCOLOR3UIVPROC glSecondaryColor3uiv;
PFNGLSECONDARYCOLOR3USPROC glSecondaryColor3us;
PFNGLSECONDARYCOLOR3USVPROC glSecondaryColor3usv;
PFNGLSECONDARYCOLORPOINTERPROC glSecondaryColorPointer;
PFNGLWINDOWPOS2DPROC glWindowPos2d;
PFNGLWINDOWPOS2DVPROC glWindowPos2dv;
PFNGLWINDOWPOS2FPROC glWindowPos2f;
PFNGLWINDOWPOS2FVPROC glWindowPos2fv;
PFNGLWINDOWPOS2IPROC glWindowPos2i;
PFNGLWINDOWPOS2IVPROC glWindowPos2iv;
PFNGLWINDOWPOS2SPROC glWindowPos2s;
PFNGLWINDOWPOS2SVPROC glWindowPos2sv;
PFNGLWINDOWPOS3DPROC glWindowPos3d;
PFNGLWINDOWPOS3DVPROC glWindowPos3dv;
PFNGLWINDOWPOS3FPROC glWindowPos3f;
PFNGLWINDOWPOS3FVPROC glWindowPos3fv;
PFNGLWINDOWPOS3IPROC glWindowPos3i;
PFNGLWINDOWPOS3IVPROC glWindowPos3iv;
PFNGLWINDOWPOS3SPROC glWindowPos3s;
PFNGLWINDOWPOS3SVPROC glWindowPos3sv;
PFNGLBLENDCOLORPROC glBlendColor;
PFNGLBLENDEQUATIONPROC glBlendEquation;


// GL1.5
PFNGLGENQUERIESPROC glGenQueries;
PFNGLDELETEQUERIESPROC glDeleteQueries;
PFNGLISQUERYPROC glIsQuery;
PFNGLBEGINQUERYPROC glBeginQuery;
PFNGLENDQUERYPROC glEndQuery;
PFNGLGETQUERYIVPROC glGetQueryiv;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLISBUFFERPROC glIsBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;


// GL2.0
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
PFNGLDRAWBUFFERSPROC glDrawBuffers;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERSOURCEPROC glGetShaderSource;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETUNIFORMFVPROC glGetUniformfv;
PFNGLGETUNIFORMIVPROC glGetUniformiv;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
PFNGLISPROGRAMPROC glIsProgram;
PFNGLISSHADERPROC glIsShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORM2IVPROC glUniform2iv;
PFNGLUNIFORM3IVPROC glUniform3iv;
PFNGLUNIFORM4IVPROC glUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv;
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;


// GL2.1
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;


// GL3.0
PFNGLCOLORMASKIPROC glColorMaski;
PFNGLGETBOOLEANI_VPROC glGetBooleani_v;
PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
PFNGLENABLEIPROC glEnablei;
PFNGLDISABLEIPROC glDisablei;
PFNGLISENABLEDIPROC glIsEnabledi;
PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
PFNGLCLAMPCOLORPROC glClampColor;
PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender;
PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv;
PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv;
PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;
PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;
PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;
PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui;
PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui;
PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui;
PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui;
PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv;
PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv;
PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv;
PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv;
PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv;
PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv;
PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv;
PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv;
PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv;
PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv;
PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv;
PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv;
PFNGLGETUNIFORMUIVPROC glGetUniformuiv;
PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation;
PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation;
PFNGLUNIFORM1UIPROC glUniform1ui;
PFNGLUNIFORM2UIPROC glUniform2ui;
PFNGLUNIFORM3UIPROC glUniform3ui;
PFNGLUNIFORM4UIPROC glUniform4ui;
PFNGLUNIFORM1UIVPROC glUniform1uiv;
PFNGLUNIFORM2UIVPROC glUniform2uiv;
PFNGLUNIFORM3UIVPROC glUniform3uiv;
PFNGLUNIFORM4UIVPROC glUniform4uiv;
PFNGLTEXPARAMETERIIVPROC glTexParameterIiv;
PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv;
PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv;
PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv;
PFNGLCLEARBUFFERIVPROC glClearBufferiv;
PFNGLCLEARBUFFERUIVPROC glClearBufferuiv;
PFNGLCLEARBUFFERFVPROC glClearBufferfv;
PFNGLCLEARBUFFERFIPROC glClearBufferfi;
PFNGLGETSTRINGIPROC glGetStringi;
PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLISVERTEXARRAYPROC glIsVertexArray;


// GL3.1
PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
PFNGLTEXBUFFERPROC glTexBuffer;
PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;
PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;


// GL3.2
PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;
PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
PFNGLFENCESYNCPROC glFenceSync;
PFNGLISSYNCPROC glIsSync;
PFNGLDELETESYNCPROC glDeleteSync;
PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
PFNGLWAITSYNCPROC glWaitSync;
PFNGLGETINTEGER64VPROC glGetInteger64v;
PFNGLGETSYNCIVPROC glGetSynciv;
PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
PFNGLSAMPLEMASKIPROC glSampleMaski;

// GL3.3
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex;
PFNGLGENSAMPLERSPROC glGenSamplers;
PFNGLDELETESAMPLERSPROC glDeleteSamplers;
PFNGLISSAMPLERPROC glIsSampler;
PFNGLBINDSAMPLERPROC glBindSampler;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;
PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv;
PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv;
PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv;
PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv;
PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv;
PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv;
PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;
PFNGLQUERYCOUNTERPROC glQueryCounter;
PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v;
PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui;
PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv;
PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui;
PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv;
PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui;
PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv;
PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui;
PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv;
PFNGLVERTEXP2UIPROC glVertexP2ui;
PFNGLVERTEXP2UIVPROC glVertexP2uiv;
PFNGLVERTEXP3UIPROC glVertexP3ui;
PFNGLVERTEXP3UIVPROC glVertexP3uiv;
PFNGLVERTEXP4UIPROC glVertexP4ui;
PFNGLVERTEXP4UIVPROC glVertexP4uiv;
PFNGLTEXCOORDP1UIPROC glTexCoordP1ui;
PFNGLTEXCOORDP1UIVPROC glTexCoordP1uiv;
PFNGLTEXCOORDP2UIPROC glTexCoordP2ui;
PFNGLTEXCOORDP2UIVPROC glTexCoordP2uiv;
PFNGLTEXCOORDP3UIPROC glTexCoordP3ui;
PFNGLTEXCOORDP3UIVPROC glTexCoordP3uiv;
PFNGLTEXCOORDP4UIPROC glTexCoordP4ui;
PFNGLTEXCOORDP4UIVPROC glTexCoordP4uiv;
PFNGLMULTITEXCOORDP1UIPROC glMultiTexCoordP1ui;
PFNGLMULTITEXCOORDP1UIVPROC glMultiTexCoordP1uiv;
PFNGLMULTITEXCOORDP2UIPROC glMultiTexCoordP2ui;
PFNGLMULTITEXCOORDP2UIVPROC glMultiTexCoordP2uiv;
PFNGLMULTITEXCOORDP3UIPROC glMultiTexCoordP3ui;
PFNGLMULTITEXCOORDP3UIVPROC glMultiTexCoordP3uiv;
PFNGLMULTITEXCOORDP4UIPROC glMultiTexCoordP4ui;
PFNGLMULTITEXCOORDP4UIVPROC glMultiTexCoordP4uiv;
PFNGLNORMALP3UIPROC glNormalP3ui;
PFNGLNORMALP3UIVPROC glNormalP3uiv;
PFNGLCOLORP3UIPROC glColorP3ui;
PFNGLCOLORP3UIVPROC glColorP3uiv;
PFNGLCOLORP4UIPROC glColorP4ui;
PFNGLCOLORP4UIVPROC glColorP4uiv;
PFNGLSECONDARYCOLORP3UIPROC glSecondaryColorP3ui;
PFNGLSECONDARYCOLORP3UIVPROC glSecondaryColorP3uiv;


namespace 
{
    void LoadGLExtensions()
    {
        // GL1.2
        glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)GetAnyGLFuncAddress("glDrawRangeElements");
        glTexImage3D = (PFNGLTEXIMAGE3DPROC)GetAnyGLFuncAddress("glTexImage3D");
        glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)GetAnyGLFuncAddress("glTexSubImage3D");
        glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)GetAnyGLFuncAddress("glCopyTexSubImage3D");

        // GL1.3
        glActiveTexture = (PFNGLACTIVETEXTUREPROC)GetAnyGLFuncAddress("glActiveTexture");
        glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)GetAnyGLFuncAddress("glSampleCoverage");
        glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)GetAnyGLFuncAddress("glCompressedTexImage3D");
        glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)GetAnyGLFuncAddress("glCompressedTexImage2D");
        glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)GetAnyGLFuncAddress("glCompressedTexImage1D");
        glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)GetAnyGLFuncAddress("glCompressedTexSubImage3D");
        glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)GetAnyGLFuncAddress("glCompressedTexSubImage2D");
        glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)GetAnyGLFuncAddress("glCompressedTexSubImage1D");
        glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)GetAnyGLFuncAddress("glGetCompressedTexImage");
        glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)GetAnyGLFuncAddress("glClientActiveTexture");
        glMultiTexCoord1d = (PFNGLMULTITEXCOORD1DPROC)GetAnyGLFuncAddress("glMultiTexCoord1d");
        glMultiTexCoord1dv = (PFNGLMULTITEXCOORD1DVPROC)GetAnyGLFuncAddress("glMultiTexCoord1dv");
        glMultiTexCoord1f = (PFNGLMULTITEXCOORD1FPROC)GetAnyGLFuncAddress("glMultiTexCoord1f");
        glMultiTexCoord1fv = (PFNGLMULTITEXCOORD1FVPROC)GetAnyGLFuncAddress("glMultiTexCoord1fv");
        glMultiTexCoord1i = (PFNGLMULTITEXCOORD1IPROC)GetAnyGLFuncAddress("glMultiTexCoord1i");
        glMultiTexCoord1iv = (PFNGLMULTITEXCOORD1IVPROC)GetAnyGLFuncAddress("glMultiTexCoord1iv");
        glMultiTexCoord1s = (PFNGLMULTITEXCOORD1SPROC)GetAnyGLFuncAddress("glMultiTexCoord1s");
        glMultiTexCoord1sv = (PFNGLMULTITEXCOORD1SVPROC)GetAnyGLFuncAddress("glMultiTexCoord1sv");
        glMultiTexCoord2d = (PFNGLMULTITEXCOORD2DPROC)GetAnyGLFuncAddress("glMultiTexCoord2d");
        glMultiTexCoord2dv = (PFNGLMULTITEXCOORD2DVPROC)GetAnyGLFuncAddress("glMultiTexCoord2dv");
        glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC)GetAnyGLFuncAddress("glMultiTexCoord2f");
        glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVPROC)GetAnyGLFuncAddress("glMultiTexCoord2fv");
        glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC)GetAnyGLFuncAddress("glMultiTexCoord2i");
        glMultiTexCoord2iv = (PFNGLMULTITEXCOORD2IVPROC)GetAnyGLFuncAddress("glMultiTexCoord2iv");
        glMultiTexCoord2s = (PFNGLMULTITEXCOORD2SPROC)GetAnyGLFuncAddress("glMultiTexCoord2s");
        glMultiTexCoord2sv = (PFNGLMULTITEXCOORD2SVPROC)GetAnyGLFuncAddress("glMultiTexCoord2sv");
        glMultiTexCoord3d = (PFNGLMULTITEXCOORD3DPROC)GetAnyGLFuncAddress("glMultiTexCoord3d");
        glMultiTexCoord3dv = (PFNGLMULTITEXCOORD3DVPROC)GetAnyGLFuncAddress("glMultiTexCoord3dv");
        glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FPROC)GetAnyGLFuncAddress("glMultiTexCoord3f");
        glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVPROC)GetAnyGLFuncAddress("glMultiTexCoord3fv");
        glMultiTexCoord3i = (PFNGLMULTITEXCOORD3IPROC)GetAnyGLFuncAddress("glMultiTexCoord3i");
        glMultiTexCoord3iv = (PFNGLMULTITEXCOORD3IVPROC)GetAnyGLFuncAddress("glMultiTexCoord3iv");
        glMultiTexCoord3s = (PFNGLMULTITEXCOORD3SPROC)GetAnyGLFuncAddress("glMultiTexCoord3s");
        glMultiTexCoord3sv = (PFNGLMULTITEXCOORD3SVPROC)GetAnyGLFuncAddress("glMultiTexCoord3sv");
        glMultiTexCoord4d = (PFNGLMULTITEXCOORD4DPROC)GetAnyGLFuncAddress("glMultiTexCoord4d");
        glMultiTexCoord4dv = (PFNGLMULTITEXCOORD4DVPROC)GetAnyGLFuncAddress("glMultiTexCoord4dv");
        glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC)GetAnyGLFuncAddress("glMultiTexCoord4f");
        glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVPROC)GetAnyGLFuncAddress("glMultiTexCoord4fv");
        glMultiTexCoord4i = (PFNGLMULTITEXCOORD4IPROC)GetAnyGLFuncAddress("glMultiTexCoord4i");
        glMultiTexCoord4iv = (PFNGLMULTITEXCOORD4IVPROC)GetAnyGLFuncAddress("glMultiTexCoord4iv");
        glMultiTexCoord4s = (PFNGLMULTITEXCOORD4SPROC)GetAnyGLFuncAddress("glMultiTexCoord4s");
        glMultiTexCoord4sv = (PFNGLMULTITEXCOORD4SVPROC)GetAnyGLFuncAddress("glMultiTexCoord4sv");
        glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC)GetAnyGLFuncAddress("glLoadTransposeMatrixf");
        glLoadTransposeMatrixd = (PFNGLLOADTRANSPOSEMATRIXDPROC)GetAnyGLFuncAddress("glLoadTransposeMatrixd");
        glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC)GetAnyGLFuncAddress("glMultTransposeMatrixf");
        glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC)GetAnyGLFuncAddress("glMultTransposeMatrixd");

        // GL1.4
        glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)GetAnyGLFuncAddress("glBlendFuncSeparate");
        glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)GetAnyGLFuncAddress("glMultiDrawArrays");
        glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)GetAnyGLFuncAddress("glMultiDrawElements");
        glPointParameterf = (PFNGLPOINTPARAMETERFPROC)GetAnyGLFuncAddress("glPointParameterf");
        glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)GetAnyGLFuncAddress("glPointParameterfv");
        glPointParameteri = (PFNGLPOINTPARAMETERIPROC)GetAnyGLFuncAddress("glPointParameteri");
        glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)GetAnyGLFuncAddress("glPointParameteriv");
        glFogCoordf = (PFNGLFOGCOORDFPROC)GetAnyGLFuncAddress("glFogCoordf");
        glFogCoordfv = (PFNGLFOGCOORDFVPROC)GetAnyGLFuncAddress("glFogCoordfv");
        glFogCoordd = (PFNGLFOGCOORDDPROC)GetAnyGLFuncAddress("glFogCoordd");
        glFogCoorddv = (PFNGLFOGCOORDDVPROC)GetAnyGLFuncAddress("glFogCoorddv");
        glFogCoordPointer = (PFNGLFOGCOORDPOINTERPROC)GetAnyGLFuncAddress("glFogCoordPointer");
        glSecondaryColor3b = (PFNGLSECONDARYCOLOR3BPROC)GetAnyGLFuncAddress("glSecondaryColor3b");
        glSecondaryColor3bv = (PFNGLSECONDARYCOLOR3BVPROC)GetAnyGLFuncAddress("glSecondaryColor3bv");
        glSecondaryColor3d = (PFNGLSECONDARYCOLOR3DPROC)GetAnyGLFuncAddress("glSecondaryColor3d");
        glSecondaryColor3dv = (PFNGLSECONDARYCOLOR3DVPROC)GetAnyGLFuncAddress("glSecondaryColor3dv");
        glSecondaryColor3f = (PFNGLSECONDARYCOLOR3FPROC)GetAnyGLFuncAddress("glSecondaryColor3f");
        glSecondaryColor3fv = (PFNGLSECONDARYCOLOR3FVPROC)GetAnyGLFuncAddress("glSecondaryColor3fv");
        glSecondaryColor3i = (PFNGLSECONDARYCOLOR3IPROC)GetAnyGLFuncAddress("glSecondaryColor3i");
        glSecondaryColor3iv = (PFNGLSECONDARYCOLOR3IVPROC)GetAnyGLFuncAddress("glSecondaryColor3iv");
        glSecondaryColor3s = (PFNGLSECONDARYCOLOR3SPROC)GetAnyGLFuncAddress("glSecondaryColor3s");
        glSecondaryColor3sv = (PFNGLSECONDARYCOLOR3SVPROC)GetAnyGLFuncAddress("glSecondaryColor3sv");
        glSecondaryColor3ub = (PFNGLSECONDARYCOLOR3UBPROC)GetAnyGLFuncAddress("glSecondaryColor3ub");
        glSecondaryColor3ubv = (PFNGLSECONDARYCOLOR3UBVPROC)GetAnyGLFuncAddress("glSecondaryColor3ubv");
        glSecondaryColor3ui = (PFNGLSECONDARYCOLOR3UIPROC)GetAnyGLFuncAddress("glSecondaryColor3ui");
        glSecondaryColor3uiv = (PFNGLSECONDARYCOLOR3UIVPROC)GetAnyGLFuncAddress("glSecondaryColor3uiv");
        glSecondaryColor3us = (PFNGLSECONDARYCOLOR3USPROC)GetAnyGLFuncAddress("glSecondaryColor3us");
        glSecondaryColor3usv = (PFNGLSECONDARYCOLOR3USVPROC)GetAnyGLFuncAddress("glSecondaryColor3usv");
        glSecondaryColorPointer = (PFNGLSECONDARYCOLORPOINTERPROC)GetAnyGLFuncAddress("glSecondaryColorPointer");
        glWindowPos2d = (PFNGLWINDOWPOS2DPROC)GetAnyGLFuncAddress("glWindowPos2d");
        glWindowPos2dv = (PFNGLWINDOWPOS2DVPROC)GetAnyGLFuncAddress("glWindowPos2dv");
        glWindowPos2f = (PFNGLWINDOWPOS2FPROC)GetAnyGLFuncAddress("glWindowPos2f");
        glWindowPos2fv = (PFNGLWINDOWPOS2FVPROC)GetAnyGLFuncAddress("glWindowPos2fv");
        glWindowPos2i = (PFNGLWINDOWPOS2IPROC)GetAnyGLFuncAddress("glWindowPos2i");
        glWindowPos2iv = (PFNGLWINDOWPOS2IVPROC)GetAnyGLFuncAddress("glWindowPos2iv");
        glWindowPos2s = (PFNGLWINDOWPOS2SPROC)GetAnyGLFuncAddress("glWindowPos2s");
        glWindowPos2sv = (PFNGLWINDOWPOS2SVPROC)GetAnyGLFuncAddress("glWindowPos2sv");
        glWindowPos3d = (PFNGLWINDOWPOS3DPROC)GetAnyGLFuncAddress("glWindowPos3d");
        glWindowPos3dv = (PFNGLWINDOWPOS3DVPROC)GetAnyGLFuncAddress("glWindowPos3dv");
        glWindowPos3f = (PFNGLWINDOWPOS3FPROC)GetAnyGLFuncAddress("glWindowPos3f");
        glWindowPos3fv = (PFNGLWINDOWPOS3FVPROC)GetAnyGLFuncAddress("glWindowPos3fv");
        glWindowPos3i = (PFNGLWINDOWPOS3IPROC)GetAnyGLFuncAddress("glWindowPos3i");
        glWindowPos3iv = (PFNGLWINDOWPOS3IVPROC)GetAnyGLFuncAddress("glWindowPos3iv");
        glWindowPos3s = (PFNGLWINDOWPOS3SPROC)GetAnyGLFuncAddress("glWindowPos3s");
        glWindowPos3sv = (PFNGLWINDOWPOS3SVPROC)GetAnyGLFuncAddress("glWindowPos3sv");
        glBlendColor = (PFNGLBLENDCOLORPROC)GetAnyGLFuncAddress("glBlendColor");
        glBlendEquation = (PFNGLBLENDEQUATIONPROC)GetAnyGLFuncAddress("glBlendEquation");


        // GL1.5
        glGenQueries = (PFNGLGENQUERIESPROC)GetAnyGLFuncAddress("glGenQueries");
        glDeleteQueries = (PFNGLDELETEQUERIESPROC)GetAnyGLFuncAddress("glDeleteQueries");
        glIsQuery = (PFNGLISQUERYPROC)GetAnyGLFuncAddress("glIsQuery");
        glBeginQuery = (PFNGLBEGINQUERYPROC)GetAnyGLFuncAddress("glBeginQuery");
        glEndQuery = (PFNGLENDQUERYPROC)GetAnyGLFuncAddress("glEndQuery");
        glGetQueryiv = (PFNGLGETQUERYIVPROC)GetAnyGLFuncAddress("glGetQueryiv");
        glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)GetAnyGLFuncAddress("glGetQueryObjectiv");
        glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)GetAnyGLFuncAddress("glGetQueryObjectuiv");
        glBindBuffer = (PFNGLBINDBUFFERPROC)GetAnyGLFuncAddress("glBindBuffer");
        glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GetAnyGLFuncAddress("glDeleteBuffers");
        glGenBuffers = (PFNGLGENBUFFERSPROC)GetAnyGLFuncAddress("glGenBuffers");
        glIsBuffer = (PFNGLISBUFFERPROC)GetAnyGLFuncAddress("glIsBuffer");
        glBufferData = (PFNGLBUFFERDATAPROC)GetAnyGLFuncAddress("glBufferData");
        glBufferSubData = (PFNGLBUFFERSUBDATAPROC)GetAnyGLFuncAddress("glBufferSubData");
        glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)GetAnyGLFuncAddress("glGetBufferSubData");
        glMapBuffer = (PFNGLMAPBUFFERPROC)GetAnyGLFuncAddress("glMapBuffer");
        glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)GetAnyGLFuncAddress("glUnmapBuffer");
        glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)GetAnyGLFuncAddress("glGetBufferParameteriv");
        glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)GetAnyGLFuncAddress("glGetBufferPointerv");


        // GL2.0
        glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)GetAnyGLFuncAddress("glBlendEquationSeparate");
        glDrawBuffers = (PFNGLDRAWBUFFERSPROC)GetAnyGLFuncAddress("glDrawBuffers");
        glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)GetAnyGLFuncAddress("glStencilOpSeparate");
        glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)GetAnyGLFuncAddress("glStencilFuncSeparate");
        glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)GetAnyGLFuncAddress("glStencilMaskSeparate");
        glAttachShader = (PFNGLATTACHSHADERPROC)GetAnyGLFuncAddress("glAttachShader");
        glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)GetAnyGLFuncAddress("glBindAttribLocation");
        glCompileShader = (PFNGLCOMPILESHADERPROC)GetAnyGLFuncAddress("glCompileShader");
        glCreateProgram = (PFNGLCREATEPROGRAMPROC)GetAnyGLFuncAddress("glCreateProgram");
        glCreateShader = (PFNGLCREATESHADERPROC)GetAnyGLFuncAddress("glCreateShader");
        glDeleteProgram = (PFNGLDELETEPROGRAMPROC)GetAnyGLFuncAddress("glDeleteProgram");
        glDeleteShader = (PFNGLDELETESHADERPROC)GetAnyGLFuncAddress("glDeleteShader");
        glDetachShader = (PFNGLDETACHSHADERPROC)GetAnyGLFuncAddress("glDetachShader");
        glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)GetAnyGLFuncAddress("glDisableVertexAttribArray");
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetAnyGLFuncAddress("glEnableVertexAttribArray");
        glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)GetAnyGLFuncAddress("glGetActiveAttrib");
        glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)GetAnyGLFuncAddress("glGetActiveUniform");
        glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)GetAnyGLFuncAddress("glGetAttachedShaders");
        glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)GetAnyGLFuncAddress("glGetAttribLocation");
        glGetProgramiv = (PFNGLGETPROGRAMIVPROC)GetAnyGLFuncAddress("glGetProgramiv");
        glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)GetAnyGLFuncAddress("glGetProgramInfoLog");
        glGetShaderiv = (PFNGLGETSHADERIVPROC)GetAnyGLFuncAddress("glGetShaderiv");
        glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GetAnyGLFuncAddress("glGetShaderInfoLog");
        glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)GetAnyGLFuncAddress("glGetShaderSource");
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)GetAnyGLFuncAddress("glGetUniformLocation");
        glGetUniformfv = (PFNGLGETUNIFORMFVPROC)GetAnyGLFuncAddress("glGetUniformfv");
        glGetUniformiv = (PFNGLGETUNIFORMIVPROC)GetAnyGLFuncAddress("glGetUniformiv");
        glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)GetAnyGLFuncAddress("glGetVertexAttribdv");
        glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)GetAnyGLFuncAddress("glGetVertexAttribfv");
        glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)GetAnyGLFuncAddress("glGetVertexAttribiv");
        glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)GetAnyGLFuncAddress("glGetVertexAttribPointerv");
        glIsProgram = (PFNGLISPROGRAMPROC)GetAnyGLFuncAddress("glIsProgram");
        glIsShader = (PFNGLISSHADERPROC)GetAnyGLFuncAddress("glIsShader");
        glLinkProgram = (PFNGLLINKPROGRAMPROC)GetAnyGLFuncAddress("glLinkProgram");
        glShaderSource = (PFNGLSHADERSOURCEPROC)GetAnyGLFuncAddress("glShaderSource");
        glUseProgram = (PFNGLUSEPROGRAMPROC)GetAnyGLFuncAddress("glUseProgram");
        glUniform1f = (PFNGLUNIFORM1FPROC)GetAnyGLFuncAddress("glUniform1f");
        glUniform2f = (PFNGLUNIFORM2FPROC)GetAnyGLFuncAddress("glUniform2f");
        glUniform3f = (PFNGLUNIFORM3FPROC)GetAnyGLFuncAddress("glUniform3f");
        glUniform4f = (PFNGLUNIFORM4FPROC)GetAnyGLFuncAddress("glUniform4f");
        glUniform1i = (PFNGLUNIFORM1IPROC)GetAnyGLFuncAddress("glUniform1i");
        glUniform2i = (PFNGLUNIFORM2IPROC)GetAnyGLFuncAddress("glUniform2i");
        glUniform3i = (PFNGLUNIFORM3IPROC)GetAnyGLFuncAddress("glUniform3i");
        glUniform4i = (PFNGLUNIFORM4IPROC)GetAnyGLFuncAddress("glUniform4i");
        glUniform1fv = (PFNGLUNIFORM1FVPROC)GetAnyGLFuncAddress("glUniform1fv");
        glUniform2fv = (PFNGLUNIFORM2FVPROC)GetAnyGLFuncAddress("glUniform2fv");
        glUniform3fv = (PFNGLUNIFORM3FVPROC)GetAnyGLFuncAddress("glUniform3fv");
        glUniform4fv = (PFNGLUNIFORM4FVPROC)GetAnyGLFuncAddress("glUniform4fv");
        glUniform1iv = (PFNGLUNIFORM1IVPROC)GetAnyGLFuncAddress("glUniform1iv");
        glUniform2iv = (PFNGLUNIFORM2IVPROC)GetAnyGLFuncAddress("glUniform2iv");
        glUniform3iv = (PFNGLUNIFORM3IVPROC)GetAnyGLFuncAddress("glUniform3iv");
        glUniform4iv = (PFNGLUNIFORM4IVPROC)GetAnyGLFuncAddress("glUniform4iv");
        glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)GetAnyGLFuncAddress("glUniformMatrix2fv");
        glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)GetAnyGLFuncAddress("glUniformMatrix3fv");
        glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)GetAnyGLFuncAddress("glUniformMatrix4fv");
        glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)GetAnyGLFuncAddress("glValidateProgram");
        glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)GetAnyGLFuncAddress("glVertexAttrib1d");
        glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)GetAnyGLFuncAddress("glVertexAttrib1dv");
        glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)GetAnyGLFuncAddress("glVertexAttrib1f");
        glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)GetAnyGLFuncAddress("glVertexAttrib1fv");
        glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)GetAnyGLFuncAddress("glVertexAttrib1s");
        glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)GetAnyGLFuncAddress("glVertexAttrib1sv");
        glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)GetAnyGLFuncAddress("glVertexAttrib2d");
        glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)GetAnyGLFuncAddress("glVertexAttrib2dv");
        glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)GetAnyGLFuncAddress("glVertexAttrib2f");
        glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)GetAnyGLFuncAddress("glVertexAttrib2fv");
        glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)GetAnyGLFuncAddress("glVertexAttrib2s");
        glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)GetAnyGLFuncAddress("glVertexAttrib2sv");
        glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)GetAnyGLFuncAddress("glVertexAttrib3d");
        glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)GetAnyGLFuncAddress("glVertexAttrib3dv");
        glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)GetAnyGLFuncAddress("glVertexAttrib3f");
        glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)GetAnyGLFuncAddress("glVertexAttrib3fv");
        glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)GetAnyGLFuncAddress("glVertexAttrib3s");
        glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)GetAnyGLFuncAddress("glVertexAttrib3sv");
        glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)GetAnyGLFuncAddress("glVertexAttrib4Nbv");
        glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)GetAnyGLFuncAddress("glVertexAttrib4Niv");
        glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)GetAnyGLFuncAddress("glVertexAttrib4Nsv");
        glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)GetAnyGLFuncAddress("glVertexAttrib4Nub");
        glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)GetAnyGLFuncAddress("glVertexAttrib4Nubv");
        glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)GetAnyGLFuncAddress("glVertexAttrib4Nuiv");
        glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)GetAnyGLFuncAddress("glVertexAttrib4Nusv");
        glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)GetAnyGLFuncAddress("glVertexAttrib4bv");
        glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)GetAnyGLFuncAddress("glVertexAttrib4d");
        glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)GetAnyGLFuncAddress("glVertexAttrib4dv");
        glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)GetAnyGLFuncAddress("glVertexAttrib4f");
        glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)GetAnyGLFuncAddress("glVertexAttrib4fv");
        glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)GetAnyGLFuncAddress("glVertexAttrib4iv");
        glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)GetAnyGLFuncAddress("glVertexAttrib4s");
        glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)GetAnyGLFuncAddress("glVertexAttrib4sv");
        glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)GetAnyGLFuncAddress("glVertexAttrib4ubv");
        glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)GetAnyGLFuncAddress("glVertexAttrib4uiv");
        glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)GetAnyGLFuncAddress("glVertexAttrib4usv");
        glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GetAnyGLFuncAddress("glVertexAttribPointer");


        // GL2.1
        glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)GetAnyGLFuncAddress("glUniformMatrix2x3fv");
        glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)GetAnyGLFuncAddress("glUniformMatrix3x2fv");
        glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)GetAnyGLFuncAddress("glUniformMatrix2x4fv");
        glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)GetAnyGLFuncAddress("glUniformMatrix4x2fv");
        glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)GetAnyGLFuncAddress("glUniformMatrix3x4fv");
        glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)GetAnyGLFuncAddress("glUniformMatrix4x3fv");

        // GL3.0
        glColorMaski = (PFNGLCOLORMASKIPROC)GetAnyGLFuncAddress("glColorMaski");
        glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)GetAnyGLFuncAddress("glGetBooleani_v");
        glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)GetAnyGLFuncAddress("glGetIntegeri_v");
        glEnablei = (PFNGLENABLEIPROC)GetAnyGLFuncAddress("glEnablei");
        glDisablei = (PFNGLDISABLEIPROC)GetAnyGLFuncAddress("glDisablei");
        glIsEnabledi = (PFNGLISENABLEDIPROC)GetAnyGLFuncAddress("glIsEnabledi");
        glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)GetAnyGLFuncAddress("glBeginTransformFeedback");
        glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)GetAnyGLFuncAddress("glEndTransformFeedback");
        glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)GetAnyGLFuncAddress("glBindBufferRange");
        glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)GetAnyGLFuncAddress("glBindBufferBase");
        glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)GetAnyGLFuncAddress("glTransformFeedbackVaryings");
        glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)GetAnyGLFuncAddress("glGetTransformFeedbackVarying");
        glClampColor = (PFNGLCLAMPCOLORPROC)GetAnyGLFuncAddress("glClampColor");
        glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)GetAnyGLFuncAddress("glBeginConditionalRender");
        glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)GetAnyGLFuncAddress("glEndConditionalRender");
        glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)GetAnyGLFuncAddress("glVertexAttribIPointer");
        glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)GetAnyGLFuncAddress("glGetVertexAttribIiv");
        glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)GetAnyGLFuncAddress("glGetVertexAttribIuiv");
        glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)GetAnyGLFuncAddress("glVertexAttribI1i");
        glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)GetAnyGLFuncAddress("glVertexAttribI2i");
        glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)GetAnyGLFuncAddress("glVertexAttribI3i");
        glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)GetAnyGLFuncAddress("glVertexAttribI4i");
        glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)GetAnyGLFuncAddress("glVertexAttribI1ui");
        glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)GetAnyGLFuncAddress("glVertexAttribI2ui");
        glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)GetAnyGLFuncAddress("glVertexAttribI3ui");
        glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)GetAnyGLFuncAddress("glVertexAttribI4ui");
        glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)GetAnyGLFuncAddress("glVertexAttribI1iv");
        glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)GetAnyGLFuncAddress("glVertexAttribI2iv");
        glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)GetAnyGLFuncAddress("glVertexAttribI3iv");
        glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)GetAnyGLFuncAddress("glVertexAttribI4iv");
        glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)GetAnyGLFuncAddress("glVertexAttribI1uiv");
        glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)GetAnyGLFuncAddress("glVertexAttribI2uiv");
        glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)GetAnyGLFuncAddress("glVertexAttribI3uiv");
        glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)GetAnyGLFuncAddress("glVertexAttribI4uiv");
        glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)GetAnyGLFuncAddress("glVertexAttribI4bv");
        glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)GetAnyGLFuncAddress("glVertexAttribI4sv");
        glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)GetAnyGLFuncAddress("glVertexAttribI4ubv");
        glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)GetAnyGLFuncAddress("glVertexAttribI4usv");
        glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)GetAnyGLFuncAddress("glGetUniformuiv");
        glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)GetAnyGLFuncAddress("glBindFragDataLocation");
        glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)GetAnyGLFuncAddress("glGetFragDataLocation");
        glUniform1ui = (PFNGLUNIFORM1UIPROC)GetAnyGLFuncAddress("glUniform1ui");
        glUniform2ui = (PFNGLUNIFORM2UIPROC)GetAnyGLFuncAddress("glUniform2ui");
        glUniform3ui = (PFNGLUNIFORM3UIPROC)GetAnyGLFuncAddress("glUniform3ui");
        glUniform4ui = (PFNGLUNIFORM4UIPROC)GetAnyGLFuncAddress("glUniform4ui");
        glUniform1uiv = (PFNGLUNIFORM1UIVPROC)GetAnyGLFuncAddress("glUniform1uiv");
        glUniform2uiv = (PFNGLUNIFORM2UIVPROC)GetAnyGLFuncAddress("glUniform2uiv");
        glUniform3uiv = (PFNGLUNIFORM3UIVPROC)GetAnyGLFuncAddress("glUniform3uiv");
        glUniform4uiv = (PFNGLUNIFORM4UIVPROC)GetAnyGLFuncAddress("glUniform4uiv");
        glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)GetAnyGLFuncAddress("glTexParameterIiv");
        glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)GetAnyGLFuncAddress("glTexParameterIuiv");
        glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)GetAnyGLFuncAddress("glGetTexParameterIiv");
        glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)GetAnyGLFuncAddress("glGetTexParameterIuiv");
        glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)GetAnyGLFuncAddress("glClearBufferiv");
        glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)GetAnyGLFuncAddress("glClearBufferuiv");
        glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)GetAnyGLFuncAddress("glClearBufferfv");
        glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)GetAnyGLFuncAddress("glClearBufferfi");
        glGetStringi = (PFNGLGETSTRINGIPROC)GetAnyGLFuncAddress("glGetStringi");
        glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)GetAnyGLFuncAddress("glIsRenderbuffer");
        glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)GetAnyGLFuncAddress("glBindRenderbuffer");
        glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)GetAnyGLFuncAddress("glDeleteRenderbuffers");
        glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)GetAnyGLFuncAddress("glGenRenderbuffers");
        glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)GetAnyGLFuncAddress("glRenderbufferStorage");
        glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)GetAnyGLFuncAddress("glGetRenderbufferParameteriv");
        glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)GetAnyGLFuncAddress("glIsFramebuffer");
        glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)GetAnyGLFuncAddress("glBindFramebuffer");
        glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)GetAnyGLFuncAddress("glDeleteFramebuffers");
        glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)GetAnyGLFuncAddress("glGenFramebuffers");
        glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)GetAnyGLFuncAddress("glCheckFramebufferStatus");
        glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)GetAnyGLFuncAddress("glFramebufferTexture1D");
        glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)GetAnyGLFuncAddress("glFramebufferTexture2D");
        glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)GetAnyGLFuncAddress("glFramebufferTexture3D");
        glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)GetAnyGLFuncAddress("glFramebufferRenderbuffer");
        glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)GetAnyGLFuncAddress("glGetFramebufferAttachmentParameteriv");
        glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)GetAnyGLFuncAddress("glGenerateMipmap");
        glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)GetAnyGLFuncAddress("glBlitFramebuffer");
        glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)GetAnyGLFuncAddress("glRenderbufferStorageMultisample");
        glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)GetAnyGLFuncAddress("glFramebufferTextureLayer");
        glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)GetAnyGLFuncAddress("glMapBufferRange");
        glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)GetAnyGLFuncAddress("glFlushMappedBufferRange");
        glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)GetAnyGLFuncAddress("glBindVertexArray");
        glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)GetAnyGLFuncAddress("glDeleteVertexArrays");
        glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)GetAnyGLFuncAddress("glGenVertexArrays");
        glIsVertexArray = (PFNGLISVERTEXARRAYPROC)GetAnyGLFuncAddress("glIsVertexArray");


        // GL3.1
        glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)GetAnyGLFuncAddress("glDrawArraysInstanced");
        glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)GetAnyGLFuncAddress("glDrawElementsInstanced");
        glTexBuffer = (PFNGLTEXBUFFERPROC)GetAnyGLFuncAddress("glTexBuffer");
        glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC)GetAnyGLFuncAddress("glPrimitiveRestartIndex");
        glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)GetAnyGLFuncAddress("glCopyBufferSubData");
        glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)GetAnyGLFuncAddress("glGetUniformIndices");
        glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)GetAnyGLFuncAddress("glGetActiveUniformsiv");
        glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)GetAnyGLFuncAddress("glGetActiveUniformName");
        glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)GetAnyGLFuncAddress("glGetUniformBlockIndex");
        glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)GetAnyGLFuncAddress("glGetActiveUniformBlockiv");
        glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)GetAnyGLFuncAddress("glGetActiveUniformBlockName");
        glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)GetAnyGLFuncAddress("glUniformBlockBinding");


        // GL3.2
        glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)GetAnyGLFuncAddress("glDrawElementsBaseVertex");
        glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)GetAnyGLFuncAddress("glDrawRangeElementsBaseVertex");
        glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)GetAnyGLFuncAddress("glDrawElementsInstancedBaseVertex");
        glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)GetAnyGLFuncAddress("glMultiDrawElementsBaseVertex");
        glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)GetAnyGLFuncAddress("glProvokingVertex");
        glFenceSync = (PFNGLFENCESYNCPROC)GetAnyGLFuncAddress("glFenceSync");
        glIsSync = (PFNGLISSYNCPROC)GetAnyGLFuncAddress("glIsSync");
        glDeleteSync = (PFNGLDELETESYNCPROC)GetAnyGLFuncAddress("glDeleteSync");
        glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)GetAnyGLFuncAddress("glClientWaitSync");
        glWaitSync = (PFNGLWAITSYNCPROC)GetAnyGLFuncAddress("glWaitSync");
        glGetInteger64v = (PFNGLGETINTEGER64VPROC)GetAnyGLFuncAddress("glGetInteger64v");
        glGetSynciv = (PFNGLGETSYNCIVPROC)GetAnyGLFuncAddress("glGetSynciv");
        glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)GetAnyGLFuncAddress("glGetInteger64i_v");
        glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)GetAnyGLFuncAddress("glGetBufferParameteri64v");
        glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)GetAnyGLFuncAddress("glFramebufferTexture");
        glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)GetAnyGLFuncAddress("glTexImage2DMultisample");
        glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)GetAnyGLFuncAddress("glTexImage3DMultisample");
        glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)GetAnyGLFuncAddress("glGetMultisamplefv");
        glSampleMaski = (PFNGLSAMPLEMASKIPROC)GetAnyGLFuncAddress("glSampleMaski");

        // GL3.3
        glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)GetAnyGLFuncAddress("glBindFragDataLocationIndexed");
        glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)GetAnyGLFuncAddress("glGetFragDataIndex");
        glGenSamplers = (PFNGLGENSAMPLERSPROC)GetAnyGLFuncAddress("glGenSamplers");
        glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)GetAnyGLFuncAddress("glDeleteSamplers");
        glIsSampler = (PFNGLISSAMPLERPROC)GetAnyGLFuncAddress("glIsSampler");
        glBindSampler = (PFNGLBINDSAMPLERPROC)GetAnyGLFuncAddress("glBindSampler");
        glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)GetAnyGLFuncAddress("glSamplerParameteri");
        glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)GetAnyGLFuncAddress("glSamplerParameteriv");
        glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)GetAnyGLFuncAddress("glSamplerParameterf");
        glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)GetAnyGLFuncAddress("glSamplerParameterfv");
        glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)GetAnyGLFuncAddress("glSamplerParameterIiv");
        glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)GetAnyGLFuncAddress("glSamplerParameterIuiv");
        glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)GetAnyGLFuncAddress("glGetSamplerParameteriv");
        glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)GetAnyGLFuncAddress("glGetSamplerParameterIiv");
        glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)GetAnyGLFuncAddress("glGetSamplerParameterfv");
        glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)GetAnyGLFuncAddress("glGetSamplerParameterIuiv");
        glQueryCounter = (PFNGLQUERYCOUNTERPROC)GetAnyGLFuncAddress("glQueryCounter");
        glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)GetAnyGLFuncAddress("glGetQueryObjecti64v");
        glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)GetAnyGLFuncAddress("glGetQueryObjectui64v");
        glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)GetAnyGLFuncAddress("glVertexAttribDivisor");
        glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)GetAnyGLFuncAddress("glVertexAttribP1ui");
        glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)GetAnyGLFuncAddress("glVertexAttribP1uiv");
        glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)GetAnyGLFuncAddress("glVertexAttribP2ui");
        glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)GetAnyGLFuncAddress("glVertexAttribP2uiv");
        glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)GetAnyGLFuncAddress("glVertexAttribP3ui");
        glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)GetAnyGLFuncAddress("glVertexAttribP3uiv");
        glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)GetAnyGLFuncAddress("glVertexAttribP4ui");
        glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)GetAnyGLFuncAddress("glVertexAttribP4uiv");
        glVertexP2ui = (PFNGLVERTEXP2UIPROC)GetAnyGLFuncAddress("glVertexP2ui");
        glVertexP2uiv = (PFNGLVERTEXP2UIVPROC)GetAnyGLFuncAddress("glVertexP2uiv");
        glVertexP3ui = (PFNGLVERTEXP3UIPROC)GetAnyGLFuncAddress("glVertexP3ui");
        glVertexP3uiv = (PFNGLVERTEXP3UIVPROC)GetAnyGLFuncAddress("glVertexP3uiv");
        glVertexP4ui = (PFNGLVERTEXP4UIPROC)GetAnyGLFuncAddress("glVertexP4ui");
        glVertexP4uiv = (PFNGLVERTEXP4UIVPROC)GetAnyGLFuncAddress("glVertexP4uiv");
        glTexCoordP1ui = (PFNGLTEXCOORDP1UIPROC)GetAnyGLFuncAddress("glTexCoordP1ui");
        glTexCoordP1uiv = (PFNGLTEXCOORDP1UIVPROC)GetAnyGLFuncAddress("glTexCoordP1uiv");
        glTexCoordP2ui = (PFNGLTEXCOORDP2UIPROC)GetAnyGLFuncAddress("glTexCoordP2ui");
        glTexCoordP2uiv = (PFNGLTEXCOORDP2UIVPROC)GetAnyGLFuncAddress("glTexCoordP2uiv");
        glTexCoordP3ui = (PFNGLTEXCOORDP3UIPROC)GetAnyGLFuncAddress("glTexCoordP3ui");
        glTexCoordP3uiv = (PFNGLTEXCOORDP3UIVPROC)GetAnyGLFuncAddress("glTexCoordP3uiv");
        glTexCoordP4ui = (PFNGLTEXCOORDP4UIPROC)GetAnyGLFuncAddress("glTexCoordP4ui");
        glTexCoordP4uiv = (PFNGLTEXCOORDP4UIVPROC)GetAnyGLFuncAddress("glTexCoordP4uiv");
        glMultiTexCoordP1ui = (PFNGLMULTITEXCOORDP1UIPROC)GetAnyGLFuncAddress("glMultiTexCoordP1ui");
        glMultiTexCoordP1uiv = (PFNGLMULTITEXCOORDP1UIVPROC)GetAnyGLFuncAddress("glMultiTexCoordP1uiv");
        glMultiTexCoordP2ui = (PFNGLMULTITEXCOORDP2UIPROC)GetAnyGLFuncAddress("glMultiTexCoordP2ui");
        glMultiTexCoordP2uiv = (PFNGLMULTITEXCOORDP2UIVPROC)GetAnyGLFuncAddress("glMultiTexCoordP2uiv");
        glMultiTexCoordP3ui = (PFNGLMULTITEXCOORDP3UIPROC)GetAnyGLFuncAddress("glMultiTexCoordP3ui");
        glMultiTexCoordP3uiv = (PFNGLMULTITEXCOORDP3UIVPROC)GetAnyGLFuncAddress("glMultiTexCoordP3uiv");
        glMultiTexCoordP4ui = (PFNGLMULTITEXCOORDP4UIPROC)GetAnyGLFuncAddress("glMultiTexCoordP4ui");
        glMultiTexCoordP4uiv = (PFNGLMULTITEXCOORDP4UIVPROC)GetAnyGLFuncAddress("glMultiTexCoordP4uiv");
        glNormalP3ui = (PFNGLNORMALP3UIPROC)GetAnyGLFuncAddress("glNormalP3ui");
        glNormalP3uiv = (PFNGLNORMALP3UIVPROC)GetAnyGLFuncAddress("glNormalP3uiv");
        glColorP3ui = (PFNGLCOLORP3UIPROC)GetAnyGLFuncAddress("glColorP3ui");
        glColorP3uiv = (PFNGLCOLORP3UIVPROC)GetAnyGLFuncAddress("glColorP3uiv");
        glColorP4ui = (PFNGLCOLORP4UIPROC)GetAnyGLFuncAddress("glColorP4ui");
        glColorP4uiv = (PFNGLCOLORP4UIVPROC)GetAnyGLFuncAddress("glColorP4uiv");
        glSecondaryColorP3ui = (PFNGLSECONDARYCOLORP3UIPROC)GetAnyGLFuncAddress("glSecondaryColorP3ui");
        glSecondaryColorP3uiv = (PFNGLSECONDARYCOLORP3UIVPROC)GetAnyGLFuncAddress("glSecondaryColorP3uiv");
    }
}