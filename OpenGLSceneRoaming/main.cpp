#include "OpenGLSceneRoaming.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLSceneRoaming w;
    w.show();
    return a.exec();
}
