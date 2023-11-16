#pragma once

#include <QtWidgets/QWidget>
#include "ui_OpenGLHDR.h"
#include <Windows.h>
#include <GL/eglew.h>
#include "QtEvent.h"
#include "cshader.h"
//#include "camera.h"
#include "cutil.h"
#include "cmesh.h"
#include "ccamera.h"
#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
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


    void showEvent(QShowEvent* event);


    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);


    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
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

    //GLuint indices[6] = {
    //    0,1,2, //第一个三角形
    //    0, 2, 3 //第二个三角形
    //};
    // Indices for plane with texture
    std::vector<GLuint> indices =
    {
        0, 1, 2,
        0, 2, 3
    };
   // GLuint program;
    GLuint   tex1, tex2;

    GLint smp1, smp2;


    /// HDR
    // 加载和编译HDR着色器
    GLuint hdrShader;
    GLuint hdrFBO, hdrTexture, hdrRBO;

 

    chen::cshader* shaderProgram;
    chen::cshader* framebufferProgram;



    Shader* shader_ptr;
    Shader* framebufferprogram_ptr;

    Camera* camera_ptr;
    Mesh* mesh_ptr;
    Texture* normalMap_ptr;
    Texture *displacementMap_ptr;
    // Prepare framebuffer rectangle VBO and VAO
    GLuint rectVAO;
    GLuint rectVBO;
    // Vertices for plane with texture
    std::vector<chen::Vertex> hdrvertices =
    {
        chen::Vertex{QVector3D(-1.0f, -1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector2D(0.0f, 0.0f)},
        chen::Vertex{QVector3D(-1.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector2D(0.0f, 1.0f)},
        chen::Vertex{QVector3D(1.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector2D(1.0f, 1.0f)},
        chen::Vertex{QVector3D(1.0f, -1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector2D(1.0f, 0.0f)}
    };
    float rectangleVertices[24] =
    {
        //  Coords   // texCoords
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,

         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    chen::ccamera* camera;
    chen::ctexture* normalMap;
    chen::ctexture* displacementMap;

    GLuint FBO;
    GLuint framebufferTexture;
    GLuint RBO;



    GLuint postProcessingFBO;
    GLuint postProcessingTexture;

//=======
//>>>>>>> 57f668359059e25a807a7659f9ca066108354637
    // Number of samples per pixel for MSAA
    unsigned int samples = 8;

    // Controls the gamma function
//<<<<<<< HEAD
    float gamma = 4.2f;

    chen::cmesh* plane;
    int width = 800;
    int height = 800;
//=======
  //  float gamma = 5.2f;

 //   chen::Camera* camera = NULL;/*
//>>>>>>> 57f668359059e25a807a7659f9ca066108354637*/
};
