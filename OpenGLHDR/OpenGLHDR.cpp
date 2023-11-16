﻿#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

#include <assert.h>
#include "cutil.h"
#include <QVector4D>
#include <QVector3D>
#include <GL/glew.h>
#include "OpenGLHDR.h"
#include "ctexture.h"
#include "cvbo.h"
#include <QVector2D>
#include "cmesh.h"
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

	// 获取OpenGL版本信息
	const GLubyte* version = glGetString(GL_VERSION);
	printf("[OpenGL version = %s]\n", version);
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

void OpenGLHDR::showEvent(QShowEvent* event)
{
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


bool OpenGLHDR::initializeHDR()
{
	hdrShader = CreateGpuProgram("shader/hdr_shader.vert", "shader/hdr_shader.frag");

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

void OpenGLHDR::keyPressEvent(QKeyEvent* event)
{
	if (camera)
	{
		camera->Inputs(event);
		_gl_update();
	}
	/*if (event->key() == Qt::Key_W)
	{
		isPressW = true;
	}
	if (event->key() == Qt::Key_S)
	{
		isPressS = true;
	}
	if (event->key() == Qt::Key_A)
	{
		isPressA = true;
	}
	if (event->key() == Qt::Key_D)
	{
		isPressD = true;
	}
	if (event->key() == Qt::Key_Q)
	{
		isPressQ = true;
	}
	if (event->key() == Qt::Key_E)
	{
		isPressE = true;
	}*/
}

void OpenGLHDR::keyReleaseEvent(QKeyEvent* event)
{
	if (camera)
	{
		camera->Inputs(event);
		_gl_update();
	}
	/*if (event->key() == Qt::Key_W)
	{
		isPressW = false;
	}
	if (event->key() == Qt::Key_S)
	{
		isPressS = false;
	}
	if (event->key() == Qt::Key_A)
	{
		isPressA = false;
	}
	if (event->key() == Qt::Key_D)
	{
		isPressD = false;
	}
	if (event->key() == Qt::Key_Q)
	{
		isPressQ = false;
	}
	if (event->key() == Qt::Key_E)
	{
		isPressE = false;
	}*/
}

void OpenGLHDR::mousePressEvent(QMouseEvent* event)
{
	if (camera)
	{
		camera->Inputs(event);
		_gl_update();
	}
	//lastPoint = event->pos();
}

void OpenGLHDR::mouseMoveEvent(QMouseEvent* event)
{
	if (camera)
	{
		camera->Inputs(event);
		_gl_update();
	}
	/*QPoint delta = event->pos() - lastPoint;
	lastPoint = event->pos();

	camera.Rotate(delta.y() * rotateSpeed, delta.x() * rotateSpeed, 0);



	_gl_update();*/
}
bool OpenGLHDR::initializeGL()
{
#if HDR_VEC

	//<<<<<<< HEAD

	// Generates shaders
	shader_ptr = new  Shader("shader/default.vert", "shader/default.frag", "shader/default.geom");
	  framebufferprogram_ptr = new Shader("shader/framebuffer.vert", "shader/framebuffer.frag");

	// Take care of all the light related things
	//glm::vec4 lightColor = glm::vec4(100.0f, 100.0f, 100.0f, 1.0f);
	//glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);




	QVector4D lightColor(100.0f, 100.0f, 100.0f, 1.0f);
	QVector3D lightPos(0.5f, 0.5f, 0.5f);


	shader_ptr->Activate();
	glUniform4f(glGetUniformLocation(shader_ptr->ID, "lightColor"), lightColor.x(), lightColor.y(), lightColor.z(), lightColor.w());
	glUniform3f(glGetUniformLocation(shader_ptr->ID, "lightPos"), lightPos.x(), lightPos.y(), lightPos.z());
	framebufferprogram_ptr->Activate();
	glUniform1i(glGetUniformLocation(framebufferprogram_ptr->ID, "screenTexture"), 0);
	glUniform1f(glGetUniformLocation(framebufferprogram_ptr->ID, "gamma"), gamma);



	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Multisampling
	glEnable(GL_MULTISAMPLE);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);


	// Creates camera object
	QVector3D tempvec3d(0.0f, 0.0f, 2.0f);
	 camera_ptr = new Camera(width, height, tempvec3d/*glm::vec3(0.0f, 0.0f, 2.0f)*/);
	 /*
	* I'm doing this relative path thing in order to centralize all the resources into one folder and not
	* duplicate them between tutorial folders. You can just copy paste the resources from the 'Resources'
	* folder and then give a relative path from this folder to whatever resource you want to get to.
	* Also note that this requires C++17, so go to Project Properties, C/C++, Language, and select C++17
	*/


	// Prepare framebuffer rectangle VBO and VAO
	//unsigned int rectVAO, rectVBO;
	 glGenVertexArrays(1, &rectVAO);
	 glGenBuffers(1, &rectVBO);
	 glBindVertexArray(rectVAO);
	 glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	 glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	 glEnableVertexAttribArray(0);
	 glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	 glEnableVertexAttribArray(1);
	 glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	 // Variables to create periodic event for FPS displaying
	 double prevTime = 0.0;
	 double crntTime = 0.0;
	 double timeDiff;
	 // Keeps track of the amount of frames in timeDiff
	 unsigned int counter = 0;

	 // Use this to disable VSync (not advized)
	 //glfwSwapInterval(0);


	 // Create Frame Buffer Object
	 //unsigned int FBO;
	 glGenFramebuffers(1, &FBO);
	 glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	 // Create Framebuffer Texture
	 //unsigned int framebufferTexture;
	 glGenTextures(1, &framebufferTexture);
	 glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture);
	 glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, width, height, GL_TRUE);

	 glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	 glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	 glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	 glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture, 0);

	 // Create Render Buffer Object
	 //unsigned int RBO;
	 glGenRenderbuffers(1, &RBO);
	 glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	 glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	 glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);


	 // Error checking framebuffer
	 auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	 if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		 std::cout << "Framebuffer error: " << fboStatus << std::endl;

	 // Create Frame Buffer Object
	 //unsigned int postProcessingFBO;
	 glGenFramebuffers(1, &postProcessingFBO);
	 glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

	 // Create Framebuffer Texture
	 //unsigned int postProcessingTexture;
	 glGenTextures(1, &postProcessingTexture);
	 glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

	 // Error checking framebuffer
	 fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	 if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		 std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;




	 // Paths to textures
	 //std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	 //std::string diffusePath = "/Resources/YoutubeOpenGL 27 - Normal Maps/textures/diffuse.png.bgra";
	 //std::string normalPath = "/Resources/YoutubeOpenGL 27 - Normal Maps/textures/normal.png.bgra";
	 //std::string displacementPath = "/Resources/YoutubeOpenGL 28 - Parallax Occlusion Mapping/textures/displacement.png.bgra";

	 std::vector<Texture> textures =
	 {
		 Texture("images/video.png", "diffuse", GL_TEXTURE0)
	 };
	 // Vertices for plane with texture
	  std::vector<Vertex> verticesss =
	 { 
		 Vertex{QVector3D(-1.0f, -1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector2D(0.0f, 0.0f)},
		 Vertex{QVector3D(-1.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector2D(0.0f, 1.0f)},
		 Vertex{QVector3D(1.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector2D(1.0f, 1.0f)},
		 Vertex{QVector3D(1.0f, -1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector2D(1.0f, 0.0f)}
	 
		// Vertex{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		// Vertex{glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
		// Vertex{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
		// Vertex{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)}
	 }; 
	 // Plane with the texture
	 mesh_ptr = new  Mesh(verticesss, indices, textures);
	 // Normal map for the plane
	 normalMap_ptr = new Texture("images/normal.png", "normal", GL_TEXTURE1);
	 displacementMap_ptr = new Texture("images/displacement.png", "displacement", GL_TEXTURE2);

	 check_error();

	return true;
#else

	shaderProgram = new chen::cshader("shader/default.vert", "shader/default.frag", "shader/default.geom");
	check_error();
	framebufferProgram = new chen::cshader("shader/framebuffer.vert", "shader/framebuffer.frag");

	






	//program = CreateGpuProgram("assets/hdr/vertexShader.glsl", "assets/hdr/fragmentShader.glsl");
	// 使用着色器程序
//	//glUseProgram(program);
//=======
	//program = CreateGpuProgram("shader/default.vert", "shader/default.frag", "shader/default.geom");
	//
	//// DHR
	//GLuint HDRshader = CreateGpuProgram("shader/framebuffer.vert", "shader/framebuffer.frag");

	//QVector4D lightColor(100.0f, 100.0f, 100.0f, 1.0f);
	//QVector3D lightPos  (0.5f, 0.5f, 0.5f);
	// ʹ����ɫ������
//	 glUseProgram(program);
//>>>>>>> 57f668359059e25a807a7659f9ca066108354637
	check_error();
	//	glUniform4f(glGetUniformLocation(program, "lightColor"), lightColor.x(), lightColor.y(), lightColor.z(), lightColor.w());
	//	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x(), lightPos.y(), lightPos.z());
		//assert(!glGetError());
	//<<<<<<< HEAD
		// 获取shader 顶点pos
		//GLint posLocation = glGetAttribLocation(program, "pos");
		//GLint colorLocation = glGetAttribLocation(program, "color");
		//GLint texcoordLocation = glGetAttribLocation(program, "texcoord");


		// uniform  纹理tex1
		//smp1 = glGetUniformLocation(program, "smp1");
		////smp2 = glGetUniformLocation(program, "smp2");

		//GLint tLocation = glGetAttribLocation(program, "t");

	/*QVector4D lightColor(100.0f, 100.0f, 100.0f, 1.0f);
	*///QVector3D lightPos(0.5f, 0.5f, 0.5f);


	shaderProgram->activate();
	//glUniform4f(glGetUniformLocation(shaderProgram->programID, "lightColor"), lightColor.x(), lightColor.y(), lightColor.z(), lightColor.w());
	//glUniform3f(glGetUniformLocation(shaderProgram->programID, "lightPos"), lightPos.x(), lightPos.y(), lightPos.z());
	framebufferProgram->activate();
	glUniform1i(glGetUniformLocation(framebufferProgram->programID, "screenTexture"), 0);
	glUniform1f(glGetUniformLocation(framebufferProgram->programID, "gamma"), gamma);
	check_error();
	//=======
	//	 glUseProgram(HDRshader);
	//	 glUniform1i(glGetUniformLocation(HDRshader, "screenTexture"), 0);
	//	 glUniform1f(glGetUniformLocation(HDRshader, "gamma"), gamma);
	//
	//	 // Enables the Depth Buffer
	//	 glEnable(GL_DEPTH_TEST);
	//>>>>>>> 57f668359059e25a807a7659f9ca066108354637

		 // Enables Multisampling
		// glEnable(GL_MULTISAMPLE);

	//<<<<<<< HEAD
	check_error();



	 
	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Multisampling
	glEnable(GL_MULTISAMPLE);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);
	 
	check_error();
	camera = new chen::ccamera(width, height, QVector3D(0.0f, 0.0f, 2.0f));

	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	check_error();


	////////////////////////////////////////////////////

	// Create Frame Buffer Object
	//unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	check_error();
	// Create Framebuffer Texture
	//unsigned int framebufferTexture;
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture);
	check_error();
	// 分配多重采样纹理的存储空间，并设置其参数
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, width, height, GL_TRUE);
	check_error();
	// 设置多重采样纹理的过滤方式
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//check_error();
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//check_error();
	// 设置多重采样纹理的边缘环绕方式
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	//check_error();
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	//check_error();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture, 0);
	check_error();
	// Create Render Buffer Object
	//unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	check_error();
	// Error checking framebuffer
	auto  fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	/////////////////////////////////////////////////////////////////
	// Create Frame Buffer Object
	//unsigned int postProcessingFBO;
	glGenFramebuffers(1, &postProcessingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);
	check_error();
	// Create Framebuffer Texture
	//unsigned int postProcessingTexture;
	glGenTextures(1, &postProcessingTexture);
	glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);
	check_error();
	// Error checking framebuffer
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;



	// path to textures

	//////
	std::vector<chen::ctexture> texturess =
	{
		chen::ctexture("images/video.png", "diffuse", GL_TEXTURE0)
	};


	// Plane with the texture
	plane = new chen::cmesh(hdrvertices, indices, texturess);
	// Normal map for the plane
	//normalMap = new chen::ctexture("images/normal.png", "normal", GL_TEXTURE1);
	//displacementMap = new chen::ctexture("images/displacement.png", "displacement", GL_TEXTURE2);
	check_error();

#endif 

	//创建VAO
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	//check_error();
	//VBO = CreateGLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vertices), vertices);

	////// 绑定一下 
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//// shader --> 启用顶点属性
	//glEnableVertexAttribArray(posLocation);
	//check_error();
	//// 告诉shader 顶点数据排列
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(
	//	posLocation, // 顶点属性ID
	//	3, // 几个数据构成一组
	//	GL_FLOAT, // 数据类型
	//	GL_FALSE, // 
	//	sizeof(float) * 8, // 步长
	//	(const void*)(sizeof(float) * 0) // 偏移量,第一组数据的起始位置
	//);
	//check_error();
	//// shader --> 启用顶点属性
	//glEnableVertexAttribArray(colorLocation);
	//check_error();
	//// 告诉shader 顶点数据排列
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(colorLocation, 3,
	//	GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3));
	//// shader --> 启用顶点属性
	//glEnableVertexAttribArray(texcoordLocation);
	//check_error();
	//// 告诉shader 顶点数据排列
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(texcoordLocation, 2,
	//	GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 6));



	//check_error();
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);


	//// 创建EBO
	//EBO = CreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(indices), indices);
	////////////////////////////////VAO 解绑后操作texcoord//////////////////////////////////////////
	//QImage img = QImage("assets/we.jpg");
	//tex1 = CreateGLTexture(GL_TEXTURE_2D, img.width(), img.height(), GL_RGBA, GL_BGRA, img.bits());

	////QImage img2 = QImage("assets/texcoord/we.jpg");
	////tex2 = CreateGLTexture(GL_TEXTURE_2D, img2.width(), img2.height(), GL_RGBA, GL_BGRA, img2.bits());


	////启用面剔除
	////glEnable(GL_CULL_FACE);
	////glCullFace(GL_BACK); // GL_FRONT

	//glPolygonMode(GL_FRONT, GL_FILL);
//=======
//	 // Enables Cull Facing
//	 glEnable(GL_CULL_FACE);
//	 // Keeps front faces
//	 glCullFace(GL_FRONT);
//	 // Uses counter clock-wise standard
//	 glFrontFace(GL_CCW);
//
//
//	 camera = new chen::Camera();

	 // ��ȡshader ����pos

	//GLint posLocation = glGetAttribLocation(program, "pos");
	//GLint colorLocation = glGetAttribLocation(program, "color");
	//GLint texcoordLocation = glGetAttribLocation(program, "texcoord");


	// uniform  ����tex1
	//smp1 = glGetUniformLocation(program, "smp1");
	//smp2 = glGetUniformLocation(program, "smp2");
//>>>>>>> 57f668359059e25a807a7659f9ca066108354637

	//GLint tLocation = glGetAttribLocation(program, "t");
	//check_error();
	//����VAO
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	//check_error();
	//VBO = CreateGLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vertices), vertices);
	//
	////// ��һ�� 
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//
	//// shader --> ���ö�������
	//glEnableVertexAttribArray(posLocation);
	//check_error();
	//// ����shader ������������
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(
	//	posLocation, // ��������ID
	//	3, // �������ݹ���һ��
	//	GL_FLOAT, // ��������
	//	GL_FALSE, // 
	//	sizeof(float) * 8, // ����
	//	(const void*)(sizeof(float) * 0) // ƫ����,��һ�����ݵ���ʼλ��
	//);
	//check_error();
	//// shader --> ���ö�������
	//glEnableVertexAttribArray(colorLocation);
	//check_error();
	//// ����shader ������������
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(colorLocation, 3,
	//	GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3));
	//// shader --> ���ö�������
	//glEnableVertexAttribArray(texcoordLocation);
	//check_error();
	//// ����shader ������������
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(texcoordLocation, 2,
	//	GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 6));
	//
	//
	//
	//check_error();
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
	//
	//
	//// ����EBO
	//EBO = CreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(indices), indices);
	////////////////////////////////VAO �������texcoord//////////////////////////////////////////
	//QImage img = QImage("images/we.jpg");
	//tex1 = CreateGLTexture(GL_TEXTURE_2D, img.width(), img.height(), GL_RGBA16F, GL_BGRA, img.bits());
	//
	////QImage img2 = QImage("assets/texcoord/we.jpg");
	////tex2 = CreateGLTexture(GL_TEXTURE_2D, img2.width(), img2.height(), GL_RGBA, GL_BGRA, img2.bits());
	//
	//
	////�������޳�
	////glEnable(GL_CULL_FACE);
	////glCullFace(GL_BACK); // GL_FRONT
	//
	//glPolygonMode(GL_FRONT, GL_FILL);
	//
	//assert(!glGetError());
	return true;
}


void OpenGLHDR::Renderer()
{

#if HDR_VEC

	// Bind the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// Specify the color of the background
	//glClearColor(pow(0.07f, gamma), pow(0.13f, gamma), pow(0.17f, gamma), 1.0f);
	// Clean the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable depth testing since it's disabled when drawing the framebuffer rectangle
	glEnable(GL_DEPTH_TEST);

	// Handles camera inputs (delete this if you have disabled VSync)
	//camera.Inputs(window);
	// Updates and exports the camera matrix to the Vertex Shader
	//camera.updateMatrix(45.0f, 0.1f, 100.0f);


	shader_ptr->Activate();
	normalMap_ptr->Bind();
	glUniform1i(glGetUniformLocation(shader_ptr->ID, "normal0"), 1);
	displacementMap_ptr->Bind();
	glUniform1i(glGetUniformLocation(shader_ptr->ID, "displacement0"), 2);

	// Draw the normal model
	mesh_ptr->Draw(*shader_ptr, *camera_ptr);

	// Make it so the multisampling FBO is read while the post-processing FBO is drawn
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
	// Conclude the multisampling and copy it to the post-processing FBO
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);


	// Bind the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Draw the framebuffer rectangle
	framebufferprogram_ptr->Activate();
	glBindVertexArray(rectVAO);
	glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
	glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	SwapBuffers(dc);
	return;
#else 






	check_error();
	// Bind the custom framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		check_error();
	// Specify the color of the background
	//glClearColor(pow(0.07f, gamma), pow(0.13f, gamma), pow(0.17f, gamma), 1.0f);
	// 	char* pixels = new char[width * height * 4];
//	 glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	check_error();
	// Clean the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	check_error();
	// Enable depth testing since it's disabled when drawing the framebuffer rectangle
	glEnable(GL_DEPTH_TEST);
	check_error();
	// Handles camera inputs (delete this if you have disabled VSync)
	//camera.Inputs(window);
	// Updates and exports the camera matrix to the Vertex Shader
	//camera->updateMatrix(45.0f, 0.1f, 100.0f);
	check_error();

	shaderProgram->activate();
	normalMap->bind();
	glUniform1i(glGetUniformLocation(shaderProgram->programID, "normal0"), 1);
	displacementMap->bind();
	glUniform1i(glGetUniformLocation(shaderProgram->programID, "displacement0"), 2);
	check_error();
	// Draw the normal model
	plane->draw(shaderProgram, camera);
	check_error();
	// Make it so the multisampling FBO is read while the post-processing FBO is drawn
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
	// Conclude the multisampling and copy it to the post-processing FBO
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	check_error();
	//GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels


//	char* pixels = new char[width * height * 4];
	//glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	// Bind the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	check_error();
	// Draw the framebuffer rectangle
	framebufferProgram->activate();
	glBindVertexArray(rectVAO);
	glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
	glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	check_error();
	SwapBuffers(dc);
#endif //
}