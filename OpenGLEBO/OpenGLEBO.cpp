
#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#include "OpenGLEBO.h"
#include <assert.h>
#include "cutil.h"


#include <GL/glew.h>

void check_error()
{
	GLenum err(glGetError());
	while (err != GL_NO_ERROR)
	{
		printf("[OpenGL error = %u]\n", err);
		err = glGetError();
	}
}

OpenGLEBO::OpenGLEBO(QWidget *parent)
    : QWidget(parent, Qt::MSWindowsOwnDC)
{
	// 设置这个就不能使用QT渲染的了  就需要重新实现QPaintEngine函数
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true);

	ui.setupUi(this);
	hwnd = reinterpret_cast<HWND>(winId());
	bool ret = CreateGLContext();
	// 激活上下文
	wglMakeCurrent(dc, rc);

	if (glewInit() != GLEW_OK)
	{
		//qDebug() << "glewInit failed!";
		throw;
	}
	initializeGL();
	// 程序启动发送事件
	//QApplication::postEvent(this, new QtEvent(QtEvent::GL_Renderer));
	_gl_update();
}

OpenGLEBO::~OpenGLEBO()
{
	wglMakeCurrent(NULL, NULL);
	if (rc)
	{
		wglDeleteContext(rc);
	}
	if (dc)
	{
		ReleaseDC(hwnd, dc);
	}
}



void OpenGLEBO::resizeEvent(QResizeEvent* event)
{
	// 绘制的大小
	glViewport(0, 0, event->size().width(), event->size().height());
	_gl_update();
}



bool OpenGLEBO::event(QEvent* event)
{
	// 接受事件判断是否是自己的渲染函数
	if (event->type() == QtEvent::GL_Renderer)
	{
		Renderer();
	}
	return QWidget::event(event);
}

bool OpenGLEBO::initializeGL()
{

	GLuint program = CreateGpuProgram("assets/ebo/vertexShader.glsl", "assets/ebo/frag/fragmentShader.glsl");

	// 使用着色器程序
	glUseProgram(program);
	check_error();
	//assert(!glGetError());
	// 获取shader 顶点pos
	GLint posLocation = glGetAttribLocation(program, "pos");
	GLint colorLocation = glGetAttribLocation(program, "color");
	//GLint tLocation = glGetAttribLocation(program, "t");
	check_error();
	//创建VAO
	 glGenVertexArrays(1, &VAO);
	 glBindVertexArray(VAO);
	check_error();
	// 创建VBO
	glGenBuffers(
		1, //创建VBO的个数
		&VBO
	);
	check_error();
	// 绑定一下 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//int size = sizeof(vertices) ;
	check_error();
	glBufferData(
		GL_ARRAY_BUFFER, //缓冲类型
		sizeof(vertices2), //数据量的大小
		vertices2, //顶点数据
		GL_STATIC_DRAW
	);
	/**
	GL_STATIC_DRAW ： 数据几乎不会改变
	GL_DYNAMIC_DRAW： 数据可能会发生改变
	GL_STREAM_DRAW: 每次绘制数据都会发生改变
	*/
	check_error();
	// shader --> 启用顶点属性
	glEnableVertexAttribArray(posLocation);
	check_error();
	// 告诉shader 顶点数据排列
	//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	glVertexAttribPointer(
		posLocation, // 顶点属性ID
		3, // 几个数据构成一组
		GL_FLOAT, // 数据类型
		GL_FALSE, // 
		sizeof(float) * 6, // 步长
		(const void*)(sizeof(float) * 0) // 偏移量,第一组数据的起始位置
	);
	check_error();
	// shader --> 启用顶点属性
	glEnableVertexAttribArray(colorLocation);
	check_error();
	// 告诉shader 顶点数据排列
	//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	glVertexAttribPointer(colorLocation,  3,  
		GL_FLOAT,  GL_FALSE,  sizeof(float) * 6, (const void*)(sizeof(float) * 3) );
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//EBO绑定数据
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//释放VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	check_error();
	 glBindVertexArray(0);
	 

	//启用面剔除
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // GL_FRONT

	glPolygonMode(GL_FRONT, GL_FILL);

	//assert(!glGetError());
	return true;
}

GLuint OpenGLEBO::CreateGpuProgram(const char* vs, const char* fs)
{
	/*GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vs);
	GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fs);*/
	GLuint vsShader = _compile_sharder(GL_VERTEX_SHADER, vs);
	GLuint fsShader = _compile_sharder(GL_FRAGMENT_SHADER, fs);
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
	check_error();
	glDeleteShader(fsShader);
	check_error();
	glDeleteShader(vsShader);
	check_error();
	return shaderProgram;
}

bool OpenGLEBO::CreateGLContext()
{
	dc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	//指定结构体版本号，设置为1
	pfd.nVersion = 1;
	pfd.cColorBits = 32;
	//设置深度缓冲
	pfd.cDepthBits = 24;
	//设置模板缓冲区位深
	pfd.cStencilBits = 8;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

	int format = 0;
	format = ChoosePixelFormat(dc, &pfd);
	if (!format)
	{
		throw;
	}
	SetPixelFormat(dc, format, &pfd);
	rc = wglCreateContext(dc);
	return true;
}

bool OpenGLEBO::_gl_update()
{
	QApplication::postEvent(this, new QtEvent(QtEvent::GL_Renderer));
	return true;
}

GLint OpenGLEBO::_compile_sharder(GLenum shaderType, const char* url)
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


void OpenGLEBO::Renderer()
{
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 要绘制两个三角形要使用VAO对象
	 // 绑定VAO
	 glBindVertexArray(VAO);
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//绘制模式  三个顶点
	//  glDrawArrays(GL_TRIANGLES, 0, 6);
	// 绑定EBO
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// EBO绘制的方式 
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	//glDrawElements(GL_TRIANGLES, 3, GL_FLOAT, 0);
	// 绑定VAO2
	//glBindVertexArray(VAO2);
	//绘制模式  三个顶点
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawElements(GL_TRIANGLES, 3, GL_FLOAT, 0);
	SwapBuffers(dc);
}