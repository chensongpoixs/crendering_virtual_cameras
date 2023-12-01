#include "OpenGLTexcoord.h"
#include <QtWidgets/QApplication>
#include "cinject.h"
int main(int argc, char *argv[])
{
    chen::init_inject();
    QApplication a(argc, argv);
    OpenGLTexcoord w;
    w.show();
    return a.exec();
}
