#pragma once

#include <QtWidgets/QWidget>
#include "ui_OpenGLEBO.h"
#include <GL/eglew.h>
#include "QtEvent.h"
class OpenGLEBO : public QWidget
{
    Q_OBJECT

public:
    OpenGLEBO(QWidget *parent = nullptr);
    ~OpenGLEBO();
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
    Ui::OpenGLEBOClass ui;
    HDC dc;
    HGLRC rc;
    HWND hwnd;


    float vertices[18] = {
        // 第一个三角形
        -0.5f, 0.5f, 0.0f,   // left top 
        -0.5f, -0.5f, 0.0f, // left bottom
        0.5f, -0.5f, 0.0f   // right bottom
        // 第二个三角形
        -0.5f, 0.5f, 0.0f, // left top
        0.5f, -0.5f, 0.0f, // right bottom
        0.5f, 0.5f, 0.0f,  // right top 
    };

    float vertices2[24] = {
        // 顶点                        // 颜色
        -0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,             // left top
        -0.5f, -0.5f, 0.0f,       0.0f, 1.0f, 0.0f,                // left bottom
        0.5f, -0.5f, 0.0f,        0.0f, 0.0f, 1.0f,                    // right bottom
        0.5f, 0.5f, 0.0f,         1.0f, 1.0f, 0.0f                   // right top 
    };

    GLuint VBO, VAO;
    // 索引缓冲区对象，这个缓冲区主要用来存储的索引信信息
    GLuint  EBO;
    GLuint indices[6] = {
        0,1,2, //第一个三角形
        0, 2, 3 //第二个三角形
    };

    /*float vertices2[9] = {
       0.9f, 0.9f, 0.0f,
       0.7f, 0.7f, 0.0f,
       0.9f, 0.7f, 0.0f
    };


    GLuint VBO2, VAO2;*/
};
