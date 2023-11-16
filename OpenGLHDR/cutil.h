#ifndef _C_UTIL_H_
#define _C_UTIL_H_
#include <Windows.h>
#include <stdint.h>
#include <GL/eglew.h>

#define check_error()															\
{																				\
	GLenum err(glGetError());													\
	while (err != GL_NO_ERROR)													\
	{																			\
		printf("[%s][%d][OpenGL error = %u]\n", __FUNCTION__, __LINE__, err);	\
		err = glGetError();														\
	}																			\
}		

char* load_file_context(const char* url);
//void check_error();



GLuint  CreateGpuProgram(const char* vs, const char* fs, const char * geometryFile = NULL);
GLint Compile_sharder(GLenum shaderType, const char* url);


//

GLuint CreateGLBuffer(GLenum target, GLenum usage, GLsizeiptr size, const void* data);

GLuint CreateGLTexture(GLenum target, int width, int height, GLint internalformat, GLint format, const void* data);





#endif // _C_UTIL_H_