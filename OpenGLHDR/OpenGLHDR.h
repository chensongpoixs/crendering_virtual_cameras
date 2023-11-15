#pragma once

#include <QtWidgets/QWidget>
#include "ui_OpenGLHDR.h"
#include <Windows.h>
#include <GL/eglew.h>
#include "QtEvent.h"
#include "camera.h"
#include "cutil.h"
class OpenGLHDR : public QWidget
{
    Q_OBJECT

public:
    OpenGLHDR(QWidget *parent = nullptr);
    ~OpenGLHDR();
    //  // 设置这个就不能使用QT渲染的了  就需要重新实现QPaintEngine函数
 //setAttribute(Qt::WA_PaintOnScreen);
    virtual QPaintEngine* paintEngine() const { return NULL; }
    virtual void resizeEvent(QResizeEvent* event);
public:
    void Renderer();
    bool event(QEvent* event);


    bool initializeGL();



    bool initializeHDR();

private:

    bool CreateGLContext();

    bool _gl_update();


private:
    Ui::OpenGLHDRClass ui;


    HDC dc;
    HGLRC rc;
    HWND hwnd;




    float vertices[32] = {
        // 顶点                        // 颜色              texcoord uv
        -0.8f, 0.8f, 0.0f,       1.0f, 0.0f, 0.0f,          0.0f, 1.0f,    // left top
        -0.8f, -0.8f, 0.0f,       0.0f, 1.0f, 0.0f,         0.0f, 0.0f,       // left bottom
        0.8f, -0.8f, 0.0f,        0.0f, 0.0f, 1.0f,         1.0f, 0.0f,           // right bottom
        0.8f, 0.8f, 0.0f,         1.0f, 1.0f, 0.0f,          1.0f, 1.0f         // right top 
    };

    GLuint VBO, VAO, EBO;

    GLuint indices[6] = {
        0,1,2, //第一个三角形
        0, 2, 3 //第二个三角形
    };

    GLuint program;
    GLuint   tex1, tex2;

    GLint smp1, smp2;


    /// HDR
    // 加载和编译HDR着色器
    GLuint hdrShader;
    GLuint hdrFBO, hdrTexture, hdrRBO;


    // Number of samples per pixel for MSAA
    unsigned int samples = 8;

    // Controls the gamma function
    float gamma = 5.2f;

    chen::Camera* camera = NULL;
};
