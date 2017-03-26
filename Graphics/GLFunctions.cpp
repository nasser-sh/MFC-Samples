#include "GLFunctions.h"


using namespace graphics;


// https://www.opengl.org/wiki/Load_OpenGL_Functions
void *GetAnyGLFunction(const char *name)
{
	void *p = (void *)wglGetProcAddress(name);

	if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(module, name);
	}

	return p;
}


void CGLFunctions::LoadGLFunctions()
{
	glCreateShader = (PFNGLCREATESHADERPROC)GetAnyGLFunction("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)GetAnyGLFunction("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)GetAnyGLFunction("glCompileShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)GetAnyGLFunction("glCreateProgram");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)GetAnyGLFunction("glLinkProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)GetAnyGLFunction("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)GetAnyGLFunction("glDetachShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)GetAnyGLFunction("glDeleteShader");
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)GetAnyGLFunction("glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)GetAnyGLFunction("glBindVertexArray");
	glGenBuffers = (PFNGLGENBUFFERSPROC)GetAnyGLFunction("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)GetAnyGLFunction("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)GetAnyGLFunction("glBufferData");
	glUseProgram = (PFNGLUSEPROGRAMPROC)GetAnyGLFunction("glUseProgram");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)GetAnyGLFunction("glGetAttribLocation");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetAnyGLFunction("glEnableVertexAttribArray");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GetAnyGLFunction("glVertexAttribPointer");
}