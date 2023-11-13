#include "OpenGLHDR.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLHDR w;
    w.show();
    return a.exec();
}
