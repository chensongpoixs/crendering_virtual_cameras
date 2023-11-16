#pragma once

#include <QtWidgets/QWidget>
#include "ui_OpenGLParse3DObjectModel.h"
#include <Windows.h>
#include <GL/eglew.h>
#include "QtEvent.h"
#include "cutil.h"
#include "camera.h"
#include "cshader.h"
#include "ctexture.h"
class OpenGLParse3DObjectModel : public QWidget
{
    Q_OBJECT

public:
    OpenGLParse3DObjectModel(QWidget *parent = nullptr);
    ~OpenGLParse3DObjectModel();
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
    Ui::OpenGLParse3DObjectModelClass ui;

    HDC dc;
    HGLRC rc;
    HWND hwnd;




    

    GLuint VBO, VAO, EBO;

     
    chen::cshader1* shader = NULL;
    //GLuint program;
    chen::ctexture* tex1;
    chen::ctexture *tex2;

    //GLint smp1, smp2;


   // GLuint modelLocation;
  //  GLuint viewLocation;
  //  GLuint projLocation;


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


    struct chen::Mesh* mesh = NULL;
};
