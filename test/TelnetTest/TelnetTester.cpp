#include "TelnetTester.h"
#include <QByteArray>

TelnetTester::TelnetTester(QObject *parent) : Tester(parent)
{
    QString host = "ptt.cc";
    connect(&t, SIGNAL(processedBytes(QByteArray)),
            this, SLOT(dumpByteArray(QByteArray)));
    t.connectTo(host);
}

void TelnetTester::dumpByteArray(QByteArray array)
{
    for (int i = 0; i < array.size(); i++)
    {
         qDebug() << static_cast<int>(array.at(i));
    }
}
