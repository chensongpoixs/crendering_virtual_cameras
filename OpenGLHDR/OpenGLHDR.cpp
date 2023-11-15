#define GLEW_STATIC
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
#include "cmesh.h"
OpenGLHDR::OpenGLHDR(QWidget *parent)
    : QWidget(parent, Qt::MSWindowsOwnDC)
{
	// è®¾ç½®è¿™ä¸ªå°±ä¸èƒ½ä½¿ç”¨QTæ¸²æŸ“çš„äº†  å°±éœ€è¦é‡æ–°å®ç°QPaintEngineå‡½æ•°
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true);

	ui.setupUi(this);
	hwnd = reinterpret_cast<HWND>(winId());
	bool ret = CreateGLContext();
	// æ¿€æ´»ä¸Šä¸‹æ–‡
	wglMakeCurrent(dc, rc);

	if (glewInit() != GLEW_OK)
	{
		//qDebug() << "glewInit failed!";
		throw;
	}
	initializeGL();
	// ç¨‹åºå¯åŠ¨å‘é€äº‹ä»¶
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
	// ç»˜åˆ¶çš„å¤§å°
	glViewport(0, 0, event->size().width(), event->size().height());
	_gl_update();
}



bool OpenGLHDR::event(QEvent* event)
{
	// æ¥å—äº‹ä»¶åˆ¤æ–­æ˜¯å¦æ˜¯è‡ªå·±çš„æ¸²æŸ“å‡½æ•°
	if (event->type() == QtEvent::GL_Renderer)
	{
		Renderer();
	}
	return QWidget::event(event);
}

bool OpenGLHDR::initializeGL()
{

<<<<<<< HEAD
	check_error();

	shaderProgram = new chen::cshader("assets/hdr/default.vert", "assets/hdr/default.frag",  "assets/hdr/default.geom");
	check_error();
	framebufferProgram = new chen::cshader( "assets/hdr/framebuffer.vert", "assets/hdr/framebuffer.frag");









	//program = CreateGpuProgram("assets/hdr/vertexShader.glsl", "assets/hdr/fragmentShader.glsl");
	// ä½¿ç”¨ç€è‰²å™¨ç¨‹åº
	//glUseProgram(program);
=======
	program = CreateGpuProgram("shader/default.vert", "shader/default.frag", "shader/default.geom");
	
	// DHR
	GLuint HDRshader = CreateGpuProgram("shader/framebuffer.vert", "shader/framebuffer.frag");
	
	QVector4D lightColor(100.0f, 100.0f, 100.0f, 1.0f);
	QVector3D lightPos  (0.5f, 0.5f, 0.5f);
	// Ê¹ÓÃ×ÅÉ«Æ÷³ÌĞò
	 glUseProgram(program);
>>>>>>> 57f668359059e25a807a7659f9ca066108354637
	check_error();
	glUniform4f(glGetUniformLocation(program, "lightColor"), lightColor.x(), lightColor.y(), lightColor.z(), lightColor.w());
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x(), lightPos.y(), lightPos.z());
	//assert(!glGetError());
<<<<<<< HEAD
	// è·å–shader é¡¶ç‚¹pos
	//GLint posLocation = glGetAttribLocation(program, "pos");
	//GLint colorLocation = glGetAttribLocation(program, "color");
	//GLint texcoordLocation = glGetAttribLocation(program, "texcoord");


	// uniform  çº¹ç†tex1
	//smp1 = glGetUniformLocation(program, "smp1");
	////smp2 = glGetUniformLocation(program, "smp2");

	//GLint tLocation = glGetAttribLocation(program, "t");

	QVector4D lightColor(100.0f, 100.0f, 100.0f, 1.0f);
	QVector3D lightPos(0.5f, 0.5f, 0.5f);


	shaderProgram->activate();
	glUniform4f(glGetUniformLocation(shaderProgram->programID, "lightColor"), lightColor.x(), lightColor.y(), lightColor.z(), lightColor.w());
	glUniform3f(glGetUniformLocation(shaderProgram->programID, "lightPos"), lightPos.x(), lightPos.y(), lightPos.z());
	framebufferProgram->activate();
	glUniform1i(glGetUniformLocation(framebufferProgram->programID, "screenTexture"), 0);
	glUniform1f(glGetUniformLocation(framebufferProgram->programID, "gamma"), gamma);
=======
	 glUseProgram(HDRshader);
	 glUniform1i(glGetUniformLocation(HDRshader, "screenTexture"), 0);
	 glUniform1f(glGetUniformLocation(HDRshader, "gamma"), gamma);

	 // Enables the Depth Buffer
	 glEnable(GL_DEPTH_TEST);
>>>>>>> 57f668359059e25a807a7659f9ca066108354637

	 // Enables Multisampling
	 glEnable(GL_MULTISAMPLE);

<<<<<<< HEAD
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



	////////////////////////////////////////////////////

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

	/////////////////////////////////////////////////////////////////
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



	// path to textures

	//////
	std::vector<chen::ctexture> texturess =
	{
		chen::ctexture("assets/hdr/diffuse.png", "diffuse", GL_TEXTURE0)
	};


	// Plane with the texture
	 plane = new chen::cmesh(hdrvertices, indices, texturess);
	// Normal map for the plane
	  normalMap = new chen::ctexture("assets/hdr/normal.png", "normal", GL_TEXTURE1);
	  displacementMap = new chen::ctexture("assets/hdr/displacement.png", "displacement", GL_TEXTURE2);




	//åˆ›å»ºVAO
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	//check_error();
	//VBO = CreateGLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vertices), vertices);

	////// ç»‘å®šä¸€ä¸‹ 
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//// shader --> å¯ç”¨é¡¶ç‚¹å±æ€§
	//glEnableVertexAttribArray(posLocation);
	//check_error();
	//// å‘Šè¯‰shader é¡¶ç‚¹æ•°æ®æ’åˆ—
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(
	//	posLocation, // é¡¶ç‚¹å±æ€§ID
	//	3, // å‡ ä¸ªæ•°æ®æ„æˆä¸€ç»„
	//	GL_FLOAT, // æ•°æ®ç±»å‹
	//	GL_FALSE, // 
	//	sizeof(float) * 8, // æ­¥é•¿
	//	(const void*)(sizeof(float) * 0) // åç§»é‡,ç¬¬ä¸€ç»„æ•°æ®çš„èµ·å§‹ä½ç½®
	//);
	//check_error();
	//// shader --> å¯ç”¨é¡¶ç‚¹å±æ€§
	//glEnableVertexAttribArray(colorLocation);
	//check_error();
	//// å‘Šè¯‰shader é¡¶ç‚¹æ•°æ®æ’åˆ—
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(colorLocation, 3,
	//	GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3));
	//// shader --> å¯ç”¨é¡¶ç‚¹å±æ€§
	//glEnableVertexAttribArray(texcoordLocation);
	//check_error();
	//// å‘Šè¯‰shader é¡¶ç‚¹æ•°æ®æ’åˆ—
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(texcoordLocation, 2,
	//	GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 6));



	//check_error();
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);


	//// åˆ›å»ºEBO
	//EBO = CreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(indices), indices);
	////////////////////////////////VAO è§£ç»‘åæ“ä½œtexcoord//////////////////////////////////////////
	//QImage img = QImage("assets/we.jpg");
	//tex1 = CreateGLTexture(GL_TEXTURE_2D, img.width(), img.height(), GL_RGBA, GL_BGRA, img.bits());

	////QImage img2 = QImage("assets/texcoord/we.jpg");
	////tex2 = CreateGLTexture(GL_TEXTURE_2D, img2.width(), img2.height(), GL_RGBA, GL_BGRA, img2.bits());


	////å¯ç”¨é¢å‰”é™¤
	////glEnable(GL_CULL_FACE);
	////glCullFace(GL_BACK); // GL_FRONT

	//glPolygonMode(GL_FRONT, GL_FILL);
=======
	 // Enables Cull Facing
	 glEnable(GL_CULL_FACE);
	 // Keeps front faces
	 glCullFace(GL_FRONT);
	 // Uses counter clock-wise standard
	 glFrontFace(GL_CCW);


	 camera = new chen::Camera();

	 // »ñÈ¡shader ¶¥µãpos

	//GLint posLocation = glGetAttribLocation(program, "pos");
	//GLint colorLocation = glGetAttribLocation(program, "color");
	//GLint texcoordLocation = glGetAttribLocation(program, "texcoord");


	// uniform  ÎÆÀítex1
	//smp1 = glGetUniformLocation(program, "smp1");
	//smp2 = glGetUniformLocation(program, "smp2");
>>>>>>> 57f668359059e25a807a7659f9ca066108354637

	//GLint tLocation = glGetAttribLocation(program, "t");
	//check_error();
	//´´½¨VAO
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	//check_error();
	//VBO = CreateGLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vertices), vertices);
	//
	////// °ó¶¨Ò»ÏÂ 
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//
	//// shader --> ÆôÓÃ¶¥µãÊôĞÔ
	//glEnableVertexAttribArray(posLocation);
	//check_error();
	//// ¸æËßshader ¶¥µãÊı¾İÅÅÁĞ
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(
	//	posLocation, // ¶¥µãÊôĞÔID
	//	3, // ¼¸¸öÊı¾İ¹¹³ÉÒ»×é
	//	GL_FLOAT, // Êı¾İÀàĞÍ
	//	GL_FALSE, // 
	//	sizeof(float) * 8, // ²½³¤
	//	(const void*)(sizeof(float) * 0) // Æ«ÒÆÁ¿,µÚÒ»×éÊı¾İµÄÆğÊ¼Î»ÖÃ
	//);
	//check_error();
	//// shader --> ÆôÓÃ¶¥µãÊôĞÔ
	//glEnableVertexAttribArray(colorLocation);
	//check_error();
	//// ¸æËßshader ¶¥µãÊı¾İÅÅÁĞ
	////GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	//glVertexAttribPointer(colorLocation, 3,
	//	GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3));
	//// shader --> ÆôÓÃ¶¥µãÊôĞÔ
	//glEnableVertexAttribArray(texcoordLocation);
	//check_error();
	//// ¸æËßshader ¶¥µãÊı¾İÅÅÁĞ
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
	//// ´´½¨EBO
	//EBO = CreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(indices), indices);
	////////////////////////////////VAO ½â°óºó²Ù×÷texcoord//////////////////////////////////////////
	//QImage img = QImage("images/we.jpg");
	//tex1 = CreateGLTexture(GL_TEXTURE_2D, img.width(), img.height(), GL_RGBA16F, GL_BGRA, img.bits());
	//
	////QImage img2 = QImage("assets/texcoord/we.jpg");
	////tex2 = CreateGLTexture(GL_TEXTURE_2D, img2.width(), img2.height(), GL_RGBA, GL_BGRA, img2.bits());
	//
	//
	////ÆôÓÃÃæÌŞ³ı
	////glEnable(GL_CULL_FACE);
	////glCullFace(GL_BACK); // GL_FRONT
	//
	//glPolygonMode(GL_FRONT, GL_FILL);
	//
	//assert(!glGetError());
	return true;
}

bool OpenGLHDR::initializeHDR()
{
	hdrShader = CreateGpuProgram("assets/hdr/hdr_shader.vert", "assets/hdr/hdr_shader.frag");

	// åˆå§‹åŒ–HDRå¸§ç¼“å†²å¯¹è±¡ï¼ˆFBOï¼‰
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// åˆ›å»ºæµ®ç‚¹æ•°çº¹ç†é™„ä»¶
	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 600, 400, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTexture, 0);

	// åˆ›å»ºæ¸²æŸ“ç¼“å†²å¯¹è±¡ï¼ˆRBOï¼‰ç”¨äºæ·±åº¦å’Œæ¨¡æ¿é™„ä»¶
	glGenRenderbuffers(1, &hdrRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, hdrRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 400);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, hdrRBO);

	// æ£€æŸ¥å¸§ç¼“å†²å®Œæ•´æ€§
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
	//æŒ‡å®šç»“æ„ä½“ç‰ˆæœ¬å·ï¼Œè®¾ç½®ä¸º1
	pfd.nVersion = 1;
	pfd.cColorBits = 32;
	//è®¾ç½®æ·±åº¦ç¼“å†²
	pfd.cDepthBits = 24;
	//è®¾ç½®æ¨¡æ¿ç¼“å†²åŒºä½æ·±
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
	// Bind the custom framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// Specify the color of the background
	glClearColor(pow(0.07f, gamma), pow(0.13f, gamma), pow(0.17f, gamma), 1.0f);
	// Clean the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable depth testing since it's disabled when drawing the framebuffer rectangle
	glEnable(GL_DEPTH_TEST);

	// Handles camera inputs (delete this if you have disabled VSync)
	//camera.Inputs(window);
	// Updates and exports the camera matrix to the Vertex Shader
	camera->updateMatrix(45.0f, 0.1f, 100.0f);


	shaderProgram->activate();
	normalMap->bind();
	glUniform1i(glGetUniformLocation(shaderProgram->programID, "normal0"), 1);
	displacementMap->bind();
	glUniform1i(glGetUniformLocation(shaderProgram->programID, "displacement0"), 2);

	// Draw the normal model
	plane->draw(shaderProgram, camera);

	// Make it so the multisampling FBO is read while the post-processing FBO is drawn
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
	// Conclude the multisampling and copy it to the post-processing FBO
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);


	// Bind the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Draw the framebuffer rectangle
	framebufferProgram->activate();
	glBindVertexArray(rectVAO);
	glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
	glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	 
	SwapBuffers(dc);
}