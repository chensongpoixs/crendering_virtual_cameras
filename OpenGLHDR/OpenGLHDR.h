#pragma once

#include <QtWidgets/QWidget>
#include "ui_OpenGLHDR.h"
#include <Windows.h>
#include <GL/eglew.h>
#include "QtEvent.h"
#include "cutil.h"
class OpenGLHDR : public QWidget
{
    Q_OBJECT

public:
    OpenGLHDR(QWidget *parent = nullptr);
    ~OpenGLHDR();
    //  // ��������Ͳ���ʹ��QT��Ⱦ����  ����Ҫ����ʵ��QPaintEngine����
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
        // ����                        // ��ɫ              texcoord uv
        -0.8f, 0.8f, 0.0f,       1.0f, 0.0f, 0.0f,          0.0f, 1.0f,    // left top
        -0.8f, -0.8f, 0.0f,       0.0f, 1.0f, 0.0f,         0.0f, 0.0f,       // left bottom
        0.8f, -0.8f, 0.0f,        0.0f, 0.0f, 1.0f,         1.0f, 0.0f,           // right bottom
        0.8f, 0.8f, 0.0f,         1.0f, 1.0f, 0.0f,          1.0f, 1.0f         // right top 
    };

    GLuint VBO, VAO, EBO;

    GLuint indices[6] = {
        0,1,2, //��һ��������
        0, 2, 3 //�ڶ���������
    };

    GLuint program;
    GLuint   tex1, tex2;

    GLint smp1, smp2;


    /// HDR
    // ���غͱ���HDR��ɫ��
    GLuint hdrShader;
    GLuint hdrFBO, hdrTexture, hdrRBO;
};
