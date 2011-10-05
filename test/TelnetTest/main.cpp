#include <QtCore/QCoreApplication>
#include "TelnetTester.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TelnetTester t;

    return a.exec();
}
