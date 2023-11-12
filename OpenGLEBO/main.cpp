#include "OpenGLEBO.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLEBO w;
    w.show();
    return a.exec();
}
