#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

#include <assert.h>
#include "cutil.h"

#include <QMatrix4x4>
#include <GL/glew.h>
#include <QTimer>
#include <QDateTime> 
#include <cshader.h>
#include "OpenGLFfmpeg.h"

OpenGLFfmpeg::OpenGLFfmpeg(QWidget *parent)
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

	QTimer* timer = new QTimer(parent);
	connect(timer, &QTimer::timeout, this, &OpenGLFfmpeg::Tick);
	timer->start(20);
	camera.SetRotation(0, 180, 0);
	initializeGL();
	// 程序启动发送事件
	//QApplication::postEvent(this, new QtEvent(QtEvent::GL_Renderer));
	_gl_update();
}

OpenGLFfmpeg::~OpenGLFfmpeg()
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



void OpenGLFfmpeg::resizeEvent(QResizeEvent* event)
{
	// 绘制的大小
	glViewport(0, 0, event->size().width(), event->size().height());
	_gl_update();
}

void OpenGLFfmpeg::showEvent(QShowEvent* event)
{
	// 最小化需要OpenGL重新渲染 函数
	_gl_update();
}

void OpenGLFfmpeg::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_W)
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
	}
}

void OpenGLFfmpeg::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_W)
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
	}
	if (event->key() == Qt::Key_Space)
	{
		isVR360 = !isVR360;
	}
}

void OpenGLFfmpeg::mousePressEvent(QMouseEvent* event)
{
	lastPoint = event->pos();
}

void OpenGLFfmpeg::mouseMoveEvent(QMouseEvent* event)
{
	QPoint delta = event->pos() - lastPoint;
	lastPoint = event->pos();

	camera.Rotate(delta.y() * rotateSpeed, delta.x() * rotateSpeed, 0);



	_gl_update();
}



bool OpenGLFfmpeg::event(QEvent* event)
{
	// 接受事件判断是否是自己的渲染函数
	if (event->type() == QtEvent::GL_Renderer)
	{
		Renderer();
	}
	return QWidget::event(event);
}

bool OpenGLFfmpeg::initializeGL()
{
	video_capture_ptr = new chen::cvideo_capture();
	const char* media_url = "rtmp://ns8.indexforce.com/home/mystream";
	if (!video_capture_ptr->open(media_url, chen::PIX_FMT_YUV420P))
	{
		printf("open video url  %s failed !!!\n ", media_url);
		throw;
	}
	 quadmodel = new chen::cmodel("assets/quad.obj");


	 spheremodel = new chen::cmodel("assets/sphere.obj");
	  

	if (video_capture_ptr->formatType == chen::PIX_FMT_YUV420P)
	{
		shader = new chen::cshader1("assets/ffmpeg/vertexShader.glsl", "assets/ffmpeg/fragment_yuv.glsl");

		tex1 = new chen::ctexture(video_capture_ptr->width, video_capture_ptr->height, GL_LUMINANCE, GL_LUMINANCE, NULL);

		tex2 = new chen::ctexture(video_capture_ptr->width/2, video_capture_ptr->height/2, GL_LUMINANCE, GL_LUMINANCE, NULL);
		tex3 = new chen::ctexture(video_capture_ptr->width/2, video_capture_ptr->height/2, GL_LUMINANCE, GL_LUMINANCE, NULL);
	}
	

	//开启深度测试
	glEnable(GL_DEPTH_TEST);
	//启用面剔除
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // GL_FRONT

	glPolygonMode(GL_FRONT, GL_FILL);

	//assert(!glGetError());
	return true;
}


bool OpenGLFfmpeg::CreateGLContext()
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

bool OpenGLFfmpeg::_gl_update()
{
	QApplication::postEvent(this, new QtEvent(QtEvent::GL_Renderer));
	return true;
}


void OpenGLFfmpeg::Tick()
{

	static long long lastts = 0;
	if (lastts == 0)
	{
		lastts = QDateTime::currentMSecsSinceEpoch();
	}

	float dt = QDateTime::currentMSecsSinceEpoch() - lastts;
	lastts = QDateTime::currentMSecsSinceEpoch();
	dt /= 1000;
	if (isPressW)
	{
		camera.Translate(0, 0, speed * dt);
	}

	if (isPressS)
	{
		camera.Translate(0, 0, -speed * dt);
	}
	if (isPressA)
	{
		camera.Translate(-speed * dt, 0, 0);
	}
	if (isPressD)
	{
		camera.Translate(speed * dt, 0, 0);
	}
	if (isPressQ)
	{
		camera.Translate(0, speed * dt, 0);
	}
	if (isPressE)
	{
		camera.Translate(0, -speed * dt, 0);
	}
	_gl_update();
}
void OpenGLFfmpeg::Renderer()
{
	// 投影矩阵
	QMatrix4x4 projMat;
	//1.  float verticalAngle 垂直角度
	//, float aspectRatio, float nearPlane, float farPlane
	projMat.perspective(45, width() / (float)height(), 0.1f, 100);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



retry:
	AVFrame* frame = NULL;
	int ret = video_capture_ptr->retrieve(frame);
	if (ret < 0)
	{
		throw;
	}
	//读取文件结束位置了
	if (ret == 0)
	{
		video_capture_ptr->seek(0);
		goto retry;
	}
	printf("[pts = %u]\n", frame->pts);
	chen::cmodel* model = isVR360 ? spheremodel : quadmodel;
	//图形矩阵
	const float* _videmat = isVR360 ? camera.GetViewMat().constData() : QMatrix4x4().constData();
	// 投影矩阵
	const float* _projmat = isVR360 ? projMat.constData() : QMatrix4x4().constData();
	if (video_capture_ptr->formatType == chen::PIX_FMT_YUV420P)
	{
		tex1->update_texture2d(frame->width, frame->height, frame->linesize[0], frame->data[0]);
		tex2->update_texture2d(frame->width/2, frame->height/2, frame->linesize[1], frame->data[1]);
		tex3->update_texture2d(frame->width/2, frame->height/2, frame->linesize[2], frame->data[2]);


		model->apply_shader(shader);
		model->set_texture("smp1", tex1);

		model->set_texture("smp2", tex2);
		model->set_texture("smp3", tex3);
		// 设置角度
		//model->set_position(0, 0, -2);
		//model->set_rotation(30, 0, 1, 0);
		//model->set_scale(1.0f, 1.0f, 1.0f);


		model->draw(_videmat, _projmat);
	} 
	

	 
	
	SwapBuffers(dc);
}

