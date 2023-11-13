#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#include "OpenGLVBO.h"
#include <assert.h>
#include "cutil.h"


#include <GL/glew.h>

OpenGLVBO::OpenGLVBO(QWidget *parent)
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

void check_error( )
{
	GLenum err(glGetError());
	while (err != GL_NO_ERROR)
	{
		printf("[OpenGL error = %u]\n", err);
		err = glGetError();
	}
}

OpenGLVBO::~OpenGLVBO()
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

void OpenGLVBO::resizeEvent(QResizeEvent* event)
{
	// 绘制的大小
	glViewport(0, 0, event->size().width(), event->size().height());
	_gl_update();
}



bool OpenGLVBO::event(QEvent* event)
{
	// 接受事件判断是否是自己的渲染函数
	if (event->type() == QtEvent::GL_Renderer)
	{
		Renderer();
	}
	return QWidget::event(event);
}


const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
)";



bool OpenGLVBO::initializeGL()
{

	GLuint program =  CreateGpuProgram("assets/vbo/vertexShader.glsl", "assets/vbo/frag/fragmentShader.glsl");
	//GLuint program = CreateGpuProgram(vertexShaderSource, fragmentShaderSource);

	// 使用着色器程序
	glUseProgram(program);
	check_error();
	//assert(!glGetError());
	// 获取shader 顶点pos
	GLint posLocation = glGetAttribLocation(program, "pos");
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
		sizeof(vertices), //数据量的大小
		vertices, //顶点数据
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
		sizeof(float) * 3, // 步长
		(const void*)(sizeof(float) * 0) // 偏移量,第一组数据的起始位置
	);
	check_error();
	//释放VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	check_error();
	glBindVertexArray(0);
	
	///////////////////////////////////////////VB2//////////////////////////////////
	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);
	// 创建VBO2
	glGenBuffers(
		1, //创建VBO的个数
		&VBO2
	);
	// 绑定一下 
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	//int size = sizeof(vertices) ;

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
	// shader --> 启用顶点属性
	glEnableVertexAttribArray(posLocation);
	// 告诉shader 顶点数据排列
	//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
	glVertexAttribPointer(
		posLocation, // 顶点属性ID
		3, // 几个数据构成一组
		GL_FLOAT, // 数据类型
		GL_FALSE, // 
		sizeof(float) * 3, // 步长
		(const void*)(sizeof(float) * 0) // 偏移量,第一组数据的起始位置
	);

	//释放VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	 
	glBindVertexArray(0);

	//启用面剔除
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // GL_FRONT
	
	glPolygonMode(GL_FRONT, GL_FILL);

	//assert(!glGetError());
	return true;
}

GLuint OpenGLVBO::CreateGpuProgram(const char* vs, const char* fs)
{
	/*GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vs);
	GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fs);*/
	GLuint vsShader =  _compile_sharder(GL_VERTEX_SHADER, vs);
	GLuint fsShader = _compile_sharder(GL_FRAGMENT_SHADER, fs);
	 GLboolean vssh = glIsShader(vsShader);
	 GLboolean fssh = glIsShader(fsShader);
	check_error();
	// 创建shader 程序
	GLuint shaderProgram = glCreateProgram();
	GLboolean glprogram = glIsProgram(shaderProgram);
	//glUseProgram(shaderProgram);
	check_error(); 
	//(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders)
	// 获取已附加的着色器列表
	const GLsizei maxCount = 1024; // 最大可能的附加着色器数量
	GLsizei count = 0;
	GLuint attachedShaders[maxCount] = {0};
	glGetAttachedShaders(glprogram, maxCount, &count, attachedShaders);

	// 打印已附加的着色器列表
	printf("Attached shaders:\n"); //<< std::endl;
	for (int i = 0; i < count; ++i)
	{
		printf("attachedShaders[%u] = %u \n ", i, attachedShaders[i]);
		attachedShaders[i] = -1;
	}
	count = 0;
	// 着色器附加到着色器程序上面去 
	glAttachShader(shaderProgram, vsShader); 
	check_error(); 
	glGetAttachedShaders(glprogram, maxCount, &count, attachedShaders);

	// 打印已附加的着色器列表
	printf("Attached shaders:\n"); //<< std::endl;
	for (int i = 0; i < count; ++i) 
	{
		printf("attachedShaders[%u] = %u \n ", i, attachedShaders[i]);
		attachedShaders[i] = -1;
	}
	count = 0;
	glAttachShader(shaderProgram, fsShader);//GL_INVALID_OPERATION
	check_error();
	glGetAttachedShaders(glprogram, maxCount, &count, attachedShaders);

	// 打印已附加的着色器列表
	printf("Attached shaders:\n"); //<< std::endl;
	for (int i = 0; i < count; ++i)
	{
		printf("attachedShaders[%u] = %u \n ", i, attachedShaders[i]);
		attachedShaders[i] = -1;
	}
	count = 0;
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
	if ( success != GL_TRUE)
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
	//glValidateProgram(shaderPro);
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

bool OpenGLVBO::CreateGLContext()
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

bool OpenGLVBO::_gl_update()
{
	QApplication::postEvent(this, new QtEvent(QtEvent::GL_Renderer));
	return true;
}

GLint OpenGLVBO::_compile_sharder(GLenum shaderType, const char* url)
{
	  char* shaderCode =   load_file_context(url);
	 const char* shaderTypeStr = reinterpret_cast<const char *>("Vertex Shader");
	if (GL_FRAGMENT_SHADER == shaderType)
	{
		shaderTypeStr = reinterpret_cast<const char*>("Fragement Shader");
	} 
	//创建着色器
	GLuint shader = glCreateShader(shaderType);
	if (!shader)
	{
		throw;
	}
	if (!glIsShader(shader)) 
	{
		throw;
		// 着色器对象无效，处理错误
	}
	check_error();
	//GLint size = strlen((const char*)(shaderCode));
	//获取着色器的代码
	glShaderSource(shader, 1, &shaderCode, NULL);
	check_error();
	// 编译着色器
	glCompileShader(shader);
	check_error();
	GLint success = GL_TRUE;

	//获取编译的状态
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	check_error();
	if (GL_TRUE != success)
	{
		char infolog[1024] = { 0 };
		GLsizei loglen = 0;
		//获取错误日志
		glGetShaderInfoLog(shader, sizeof(infolog), &loglen, infolog);
		printf("[ERROR] Compile %s error %s\n",  shaderTypeStr,  infolog);

		// 编译出错删除shader
		glDeleteShader(shader);
	}
	printf("Compile Success !!!\n");
	 if (shaderCode)
	{
		delete []shaderCode;
		shaderCode = NULL;
	} 
	return shader;
}


void OpenGLVBO::Renderer()
{
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 要绘制两个三角形要使用VAO对象
	 // 绑定VAO
	glBindVertexArray(VAO);
	//绘制模式  三个顶点
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawElements(GL_TRIANGLES, 3, GL_FLOAT, 0);
	// 绑定VAO2
	glBindVertexArray(VAO2);
	//绘制模式  三个顶点
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawElements(GL_TRIANGLES, 3, GL_FLOAT, 0);
	SwapBuffers(dc);
}