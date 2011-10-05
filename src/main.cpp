#include <QtCore/QTranslator>
#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator trans;
    trans.load(":/translations/UJ_Translate");
    a.installTranslator(&trans);

    UJ::MainWindow w;
    w.show();

    return a.exec();
}
