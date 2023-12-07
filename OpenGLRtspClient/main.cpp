#include "OpenGLRtspClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLRtspClient w;
    w.show();
    return a.exec();
}
