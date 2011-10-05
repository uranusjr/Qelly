#include <QtGui/QApplication>
#include "GuiTester.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GuiTester t;

    return a.exec();
}
