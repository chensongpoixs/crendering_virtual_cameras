#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
 
#include <assert.h>
#include "cutil.h"


#include <GL/glew.h>
#include "OpenGLTexcoord.h"


OpenGLTexcoord::OpenGLTexcoord(QWidget *parent)
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

OpenGLTexcoord::~OpenGLTexcoord()
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



void OpenGLTexcoord::resizeEvent(QResizeEvent* event)
{
	// 绘制的大小
	glViewport(0, 0, event->size().width(), event->size().height());
	_gl_update();
}



bool OpenGLTexcoord::event(QEvent* event)
{
	// 接受事件判断是否是自己的渲染函数
	if (event->type() == QtEvent::GL_Renderer)
	{
		Renderer();
	}
	return QWidget::event(event);
}

bool OpenGLTexcoord::initializeGL()
{

	program = CreateGpuProgram("assets/texcoord/vertexShader.glsl", "assets/texcoord/frag/fragmentShader.glsl");

	// 使用着色器程序
	glUseProgram(program);
	check_error();
	//assert(!glGetError());
	// 获取shader 顶点pos
	GLint posLocation = glGetAttribLocation(program, "pos");
	GLint colorLocation = glGetAttribLocation(program, "color");
	GLint texcoordLocation = glGetAttribLocation(program, "texcoord");


	// uniform  纹理tex1
	smp1 = glGetUniformLocation(program, "smp1");
	//smp2 = glGetUniformLocation(program, "smp2");

	//GLint tLocation = glGetAttribLocation(program, "t");
	check_error();
	//创建VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	check_error();
	VBO = CreateGLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vertices), vertices);
	 
	//// 绑定一下 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	 
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
		sizeof(float) * 8, // 步长
		(const void*)(sizeof(float) * 0) // 偏移量,第一组数据的起始位置
	);
	check_error();
	// shader --> 启用顶点属性
	glEnableVertexAttribArray(colorLocation);
	check_error();
	// 告诉shader 顶点数据排列
	//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	glVertexAttribPointer(colorLocation, 3,
		GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3));
	// shader --> 启用顶点属性
	glEnableVertexAttribArray(texcoordLocation);
	check_error();
	// 告诉shader 顶点数据排列
	//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	glVertexAttribPointer(texcoordLocation, 2,
		GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 6));

	
	 
	check_error();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// 创建EBO
	EBO = CreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(indices), indices);
	//////////////////////////////VAO 解绑后操作texcoord//////////////////////////////////////////
	QImage img = QImage("assets/texcoord/we.jpg");
	tex1 = CreateGLTexture(GL_TEXTURE_2D, img.width(), img.height(), GL_RGBA, GL_BGRA, img.bits());
	
	//QImage img2 = QImage("assets/texcoord/we.jpg");
	//tex2 = CreateGLTexture(GL_TEXTURE_2D, img2.width(), img2.height(), GL_RGBA, GL_BGRA, img2.bits());

	
	//启用面剔除
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // GL_FRONT

	glPolygonMode(GL_FRONT, GL_FILL);

	//assert(!glGetError());
	return true;
}


bool OpenGLTexcoord::CreateGLContext()
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

bool OpenGLTexcoord::_gl_update()
{
	QApplication::postEvent(this, new QtEvent(QtEvent::GL_Renderer));
	return true;
}



void OpenGLTexcoord::Renderer()
{
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 激活shader程序
	glUseProgram(program);
	//激活纹理0号单元
	glActiveTexture(GL_TEXTURE0);
	//绑定纹理单元
	glBindTexture(GL_TEXTURE_2D, tex1);
	// 因为激活是0号单元所以使用0
	glUniform1i(smp1, 0);


	//激活纹理0号单元
	//glActiveTexture(GL_TEXTURE5);
	//绑定纹理单元
	//glBindTexture(GL_TEXTURE_2D, tex2);
	// 因为激活是0号单元所以使用0
	//glUniform1i(smp2, 5);

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