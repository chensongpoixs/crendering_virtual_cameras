#include "OpenGLTexcoord.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLTexcoord w;
    w.show();
    return a.exec();
}
