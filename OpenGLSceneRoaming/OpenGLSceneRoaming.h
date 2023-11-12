#pragma once

#include <QtWidgets/QWidget>
#include "ui_OpenGLSceneRoaming.h"
#include <Windows.h>
#include <GL/eglew.h>
#include "QtEvent.h"
#include "cutil.h"
#include "camera.h"
class OpenGLSceneRoaming : public QWidget
{
    Q_OBJECT

public:
    OpenGLSceneRoaming(QWidget *parent = nullptr);
    ~OpenGLSceneRoaming();
    //  // 设置这个就不能使用QT渲染的了  就需要重新实现QPaintEngine函数
//setAttribute(Qt::WA_PaintOnScreen);
    virtual QPaintEngine* paintEngine() const { return NULL; }
    virtual void resizeEvent(QResizeEvent* event);

    void showEvent(QShowEvent* event);


    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);


    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
public:
    void Renderer();
    bool event(QEvent* event);


    bool initializeGL();


private:

    bool CreateGLContext();

    bool _gl_update();



private slots:
    void Tick();
private:
    Ui::OpenGLSceneRoamingClass ui;

    HDC dc;
    HGLRC rc;
    HWND hwnd;




    float vertices[32] = {
        // 顶点                        // 颜色              texcoord uv
        -0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,          0.0f, 1.0f,    // left top
        -0.5f, -0.5f, 0.0f,       0.0f, 1.0f, 0.0f,         0.0f, 0.0f,       // left bottom
        0.5f, -0.5f, 0.0f,        0.0f, 0.0f, 1.0f,         1.0f, 0.0f,           // right bottom
        0.5f, 0.5f, 0.0f,         1.0f, 1.0f, 0.0f,          1.0f, 1.0f         // right top 
    };

    GLuint VBO, VAO, EBO;

    GLuint indices[6] = {
        0,1,2, //第一个三角形
        0, 2, 3 //第二个三角形
    };

    GLuint program;
    GLuint   tex1, tex2;

    GLint smp1, smp2;


    GLuint modelLocation;
    GLuint viewLocation;
    GLuint projLocation;


    chen::Camera camera;


    bool isPressW = false;   
    bool  isPressS = false;  
    bool  isPressA = false;  
    bool  isPressD = false;  
    bool  isPressQ = false; 
    bool  isPressE = false;


    float speed = 1.0f;
    float rotateSpeed = 0.2f;


    QPoint lastPoint;
};