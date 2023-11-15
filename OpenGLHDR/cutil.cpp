#include "cutil.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
char* load_file_context(const char* url)
{
	char* fileContext = NULL;
	FILE* fp = ::fopen(url, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		if (len > 0)
		{
			rewind(fp);
			fileContext = new char[len + 1];
			fread(fileContext, 1, len, fp);
			fileContext[len] = 0;
		}
	}
	return fileContext;
}
void check_error()
{
	GLenum err(glGetError());
	while (err != GL_NO_ERROR)
	{
		printf("[OpenGL error = %u]\n", err);
		err = glGetError();
	}
}
GLuint   CreateGpuProgram(const char* vs, const char* fs, const char* geometryFile)
{
	/*GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vs);
	GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fs);*/
	GLuint vsShader = Compile_sharder(GL_VERTEX_SHADER, vs);
	GLuint fsShader = Compile_sharder(GL_FRAGMENT_SHADER, fs);
	GLuint gsShader = 0;
	if (geometryFile)
	{
		gsShader = Compile_sharder(GL_GEOMETRY_SHADER, geometryFile);
	}

	//GLboolean vssh = glIsShader(vsShader);
	//GLboolean fssh = glIsShader(fsShader);
	check_error();
	// 创建shader 程序
	GLuint shaderProgram = glCreateProgram();
	check_error();
	// 着色器附加到着色器程序上面去
	//glGetAttachedShaders(shaderProgram, 5)
	glAttachShader(shaderProgram, vsShader);
	//glLinkProgram(shaderProgram);
	//glDetachShader(shaderProgram, vsShader);
	//glDeleteShader(vsShader);
	check_error();
	//GL_INVALID_OPERATION
	glAttachShader(shaderProgram, fsShader);
	if (gsShader)
	{
		glAttachShader(shaderProgram, gsShader);
	}
	check_error();
	/*GLenum err = glGetError();
	printf("GL_ATTACHED_SHADERS[err = %u]\n", err);*/
	GLint success = GL_TRUE;
	check_error();

	//assert(!glGetError());
	//链接一下着色器程序
	glLinkProgram(shaderProgram);

	check_error();
	//获取编译的状态
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	check_error();
	if (!success)
	{
		char infolog[1024] = { 0 };
		GLsizei loglen = 0;
		//获取错误日志
		glGetProgramInfoLog(shaderProgram, sizeof(infolog), &loglen, infolog);
		printf("[ERROR] Compile   error %s\n", infolog);

		// 编译出错删除shader
		glDeleteProgram(shaderProgram);
	}
	check_error();
	printf("LIne sharder program success Compile Success !!!\n");
	// //删除着色器

	glDetachShader(shaderProgram, fsShader);
	check_error();
	glDetachShader(shaderProgram, vsShader);
	if (geometryFile)
	{
		glDetachShader(shaderProgram, gsShader);
		glDeleteShader(gsShader);
	}
	check_error();
	glDeleteShader(fsShader);
	check_error();
	glDeleteShader(vsShader);
	check_error();
	return shaderProgram;
}
GLint  Compile_sharder(GLenum shaderType, const char* url)
{
	char* shaderCode = load_file_context(url);
	/*const char* shaderTypeStr = reinterpret_cast<const char *>("Vertex Shader");
	if (GL_FRAGMENT_SHADER == shaderType)
	{
		shaderTypeStr = reinterpret_cast<const char*>("Fragement Shader");
	}*/
	//创建着色器
	GLuint shader = glCreateShader(shaderType);
	if (!shader)
	{
		throw;
	}
	//GLint size = strlen((const char*)(shaderCode));
	//获取着色器的代码
	glShaderSource(shader, 1, &shaderCode, NULL);
	// 编译着色器
	glCompileShader(shader);

	GLint success = GL_TRUE;

	//获取编译的状态
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infolog[1024] = { 0 };
		GLsizei loglen = 0;
		//获取错误日志
		glGetShaderInfoLog(shader, sizeof(infolog), &loglen, infolog);
		printf("[ERROR] Compile  error %s\n", /*shaderTypeStr,*/ infolog);

		// 编译出错删除shader
		glDeleteShader(shader);
	}
	printf("Compile Success !!!\n");
	if (shaderCode)
	{
		delete[]shaderCode;
		shaderCode = NULL;
	}
	return shader;
}


GLuint CreateGLBuffer(GLenum target, GLenum usage, GLsizeiptr size, const void* data)
{
	GLuint BufferID;
	// 创建VBO
	glGenBuffers(
		1, //创建VBO的个数
		&BufferID
	);
	check_error();
	// 绑定一下 
	glBindBuffer(target, BufferID);
	//int size = sizeof(vertices) ;
	check_error();
	glBufferData(
		target, //缓冲类型
		size, //数据量的大小
		data, //顶点数据
		usage
	);
	/**
	GL_STATIC_DRAW ： 数据几乎不会改变
	GL_DYNAMIC_DRAW： 数据可能会发生改变
	GL_STREAM_DRAW: 每次绘制数据都会发生改变
	*/
	check_error();
	glBindBuffer(target, 0);
	return BufferID;
}

GLuint CreateGLTexture(GLenum target, int width, int height, GLint internalformat, GLint format, const void* data)
{
	GLuint texId;
	// 创建2D物理
	glGenTextures(1, &texId);
	glBindTexture(target, texId);
	// 设置环绕方式
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// 设置过lu方式
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(
		target, //纹理类型
		0, // 多级 纹理级别
		internalformat, // 纹理格式
		width,
		 height ,
		0,
		format, //数据格式
		GL_UNSIGNED_BYTE,
		data //图片的rgb数据 
	);
	glGenerateMipmap(target);
	glBindTexture(target, 0);
	return texId;
}