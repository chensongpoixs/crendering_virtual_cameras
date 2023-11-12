#include "OpenGLMat.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLMat w;
    w.show();
    return a.exec();
}
