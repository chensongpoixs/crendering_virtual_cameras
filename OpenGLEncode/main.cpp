#include "OpenGLEncode.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLEncode w;
    w.show();
    return a.exec();
}
