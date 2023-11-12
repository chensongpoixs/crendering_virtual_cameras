#include "OpenGLVirtualCameras.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLVirtualCameras w;
    w.show();
    return a.exec();
}
