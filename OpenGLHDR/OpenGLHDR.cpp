#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

#include <assert.h>
#include "cutil.h"


#include <GL/glew.h>
#include "OpenGLHDR.h"

OpenGLHDR::OpenGLHDR(QWidget *parent)
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

OpenGLHDR::~OpenGLHDR()
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



void OpenGLHDR::resizeEvent(QResizeEvent* event)
{
	// 绘制的大小
	glViewport(0, 0, event->size().width(), event->size().height());
	_gl_update();
}



bool OpenGLHDR::event(QEvent* event)
{
	// 接受事件判断是否是自己的渲染函数
	if (event->type() == QtEvent::GL_Renderer)
	{
		Renderer();
	}
	return QWidget::event(event);
}

bool OpenGLHDR::initializeGL()
{

	program = CreateGpuProgram("shader/vertexShader.glsl", "shader/fragmentShader.glsl");
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
	QImage img = QImage("images/we.jpg");
	tex1 = CreateGLTexture(GL_TEXTURE_2D, img.width(), img.height(), GL_RGBA16F, GL_BGRA, img.bits());

	//QImage img2 = QImage("assets/texcoord/we.jpg");
	//tex2 = CreateGLTexture(GL_TEXTURE_2D, img2.width(), img2.height(), GL_RGBA, GL_BGRA, img2.bits());


	//启用面剔除
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // GL_FRONT

	glPolygonMode(GL_FRONT, GL_FILL);

	//assert(!glGetError());
	return true;
}

bool OpenGLHDR::initializeHDR()
{
	hdrShader = CreateGpuProgram("assets/hdr/hdr_shader.vert", "assets/hdr/hdr_shader.frag");

	// 初始化HDR帧缓冲对象（FBO）
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// 创建浮点数纹理附件
	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 600, 400, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTexture, 0);

	// 创建渲染缓冲对象（RBO）用于深度和模板附件
	glGenRenderbuffers(1, &hdrRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, hdrRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 400);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, hdrRBO);

	// 检查帧缓冲完整性
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error: HDR Framebuffer is not complete!\n");// << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}


bool OpenGLHDR::CreateGLContext()
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

bool OpenGLHDR::_gl_update()
{
	QApplication::postEvent(this, new QtEvent(QtEvent::GL_Renderer));
	return true;
}



void OpenGLHDR::Renderer()
{

	 
	SwapBuffers(dc);
}