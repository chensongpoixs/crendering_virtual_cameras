#pragma once

#include <QtWidgets/QWidget>
#include "ui_OpenGLVBO.h"
#include <Windows.h>
#include "QtEvent.h"

#include <GL/eglew.h>
class OpenGLVBO : public QWidget
{
    Q_OBJECT

public:
    OpenGLVBO(QWidget *parent = nullptr);
    ~OpenGLVBO();

public:


    //  // 设置这个就不能使用QT渲染的了  就需要重新实现QPaintEngine函数
    //setAttribute(Qt::WA_PaintOnScreen);
    virtual QPaintEngine* paintEngine() const { return NULL; }
    virtual void resizeEvent(QResizeEvent* event);


public:
    void Renderer();
    bool event(QEvent* event);


    bool initializeGL();

    GLuint  CreateGpuProgram(const char* vs, const char* fs);
private:

    bool CreateGLContext();

    bool _gl_update();



    GLint _compile_sharder(GLenum shaderType, const char* url);
private:
    Ui::OpenGLVBOClass ui;


    HDC dc;
    HGLRC rc;
    HWND hwnd;


    float vertices[9] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };


    GLuint VBO, VAO;

    float vertices2[9] = {
       0.9f, 0.9f, 0.0f,
       0.7f, 0.7f, 0.0f,
       0.9f, 0.7f, 0.0f
    };


    GLuint VBO2, VAO2;
};
