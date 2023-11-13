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
	// ��������Ͳ���ʹ��QT��Ⱦ����  ����Ҫ����ʵ��QPaintEngine����
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true);

	ui.setupUi(this);
	hwnd = reinterpret_cast<HWND>(winId());
	bool ret = CreateGLContext();
	// ����������
	wglMakeCurrent(dc, rc);

	if (glewInit() != GLEW_OK)
	{
		//qDebug() << "glewInit failed!";
		throw;
	}
	initializeGL();
	// �������������¼�
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
	// ���ƵĴ�С
	glViewport(0, 0, event->size().width(), event->size().height());
	_gl_update();
}



bool OpenGLHDR::event(QEvent* event)
{
	// �����¼��ж��Ƿ����Լ�����Ⱦ����
	if (event->type() == QtEvent::GL_Renderer)
	{
		Renderer();
	}
	return QWidget::event(event);
}

bool OpenGLHDR::initializeGL()
{

	program = CreateGpuProgram("assets/hdr/vertexShader.glsl", "assets/hdr/fragmentShader.glsl");
	// ʹ����ɫ������
	glUseProgram(program);
	check_error();
	//assert(!glGetError());
	// ��ȡshader ����pos
	GLint posLocation = glGetAttribLocation(program, "pos");
	GLint colorLocation = glGetAttribLocation(program, "color");
	GLint texcoordLocation = glGetAttribLocation(program, "texcoord");


	// uniform  ����tex1
	smp1 = glGetUniformLocation(program, "smp1");
	//smp2 = glGetUniformLocation(program, "smp2");

	//GLint tLocation = glGetAttribLocation(program, "t");
	check_error();
	//����VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	check_error();
	VBO = CreateGLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vertices), vertices);

	//// ��һ�� 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// shader --> ���ö�������
	glEnableVertexAttribArray(posLocation);
	check_error();
	// ����shader ������������
	//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	glVertexAttribPointer(
		posLocation, // ��������ID
		3, // �������ݹ���һ��
		GL_FLOAT, // ��������
		GL_FALSE, // 
		sizeof(float) * 8, // ����
		(const void*)(sizeof(float) * 0) // ƫ����,��һ�����ݵ���ʼλ��
	);
	check_error();
	// shader --> ���ö�������
	glEnableVertexAttribArray(colorLocation);
	check_error();
	// ����shader ������������
	//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	glVertexAttribPointer(colorLocation, 3,
		GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3));
	// shader --> ���ö�������
	glEnableVertexAttribArray(texcoordLocation);
	check_error();
	// ����shader ������������
	//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	glVertexAttribPointer(texcoordLocation, 2,
		GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 6));



	check_error();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// ����EBO
	EBO = CreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(indices), indices);
	//////////////////////////////VAO �������texcoord//////////////////////////////////////////
	QImage img = QImage("assets/we.jpg");
	tex1 = CreateGLTexture(GL_TEXTURE_2D, img.width(), img.height(), GL_RGBA, GL_BGRA, img.bits());

	//QImage img2 = QImage("assets/texcoord/we.jpg");
	//tex2 = CreateGLTexture(GL_TEXTURE_2D, img2.width(), img2.height(), GL_RGBA, GL_BGRA, img2.bits());


	//�������޳�
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // GL_FRONT

	glPolygonMode(GL_FRONT, GL_FILL);

	//assert(!glGetError());
	return true;
}

bool OpenGLHDR::initializeHDR()
{
	hdrShader = CreateGpuProgram("assets/hdr/hdr_shader.vert", "assets/hdr/hdr_shader.frag");

	// ��ʼ��HDR֡�������FBO��
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// ����������������
	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 600, 400, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTexture, 0);

	// ������Ⱦ�������RBO��������Ⱥ�ģ�帽��
	glGenRenderbuffers(1, &hdrRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, hdrRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 400);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, hdrRBO);

	// ���֡����������
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
	//ָ���ṹ��汾�ţ�����Ϊ1
	pfd.nVersion = 1;
	pfd.cColorBits = 32;
	//������Ȼ���
	pfd.cDepthBits = 24;
	//����ģ�建����λ��
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