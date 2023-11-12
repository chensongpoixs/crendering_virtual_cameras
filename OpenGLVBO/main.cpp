#include "OpenGLVBO.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLVBO w;
    w.show();
    return a.exec();
}
