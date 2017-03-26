#pragma once
#include "PlatformGL.h"


namespace graphics
{
	class CGLFunctions
	{
	public:
		CGLFunctions() = default;
		~CGLFunctions() = default;

	protected:
		void LoadGLFunctions();

		PFNGLCREATESHADERPROC glCreateShader;
		PFNGLSHADERSOURCEPROC glShaderSource;
		PFNGLCOMPILESHADERPROC glCompileShader;
		PFNGLCREATEPROGRAMPROC glCreateProgram;
		PFNGLLINKPROGRAMPROC glLinkProgram;
		PFNGLATTACHSHADERPROC glAttachShader;
		PFNGLDETACHSHADERPROC glDetachShader;
		PFNGLDELETESHADERPROC glDeleteShader;
		PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
		PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
		PFNGLGENBUFFERSPROC glGenBuffers;
		PFNGLBINDBUFFERPROC glBindBuffer;
		PFNGLBUFFERDATAPROC glBufferData;
		PFNGLUSEPROGRAMPROC glUseProgram;
		PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
		PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
		PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	};
}
