#pragma once

#include <QtWidgets/QWidget>
#include "ui_OpenGLEncode.h"

class OpenGLEncode : public QWidget
{
    Q_OBJECT

public:
    OpenGLEncode(QWidget *parent = nullptr);
    ~OpenGLEncode();

private:
    Ui::OpenGLEncodeClass ui;
};
