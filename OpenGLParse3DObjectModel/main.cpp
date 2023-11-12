#include "OpenGLParse3DObjectModel.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLParse3DObjectModel w;
    w.show();
    return a.exec();
}
