#include "TelnetTester.h"
#include <QByteArray>
#include <QDebug>

TelnetTester::TelnetTester(QObject *parent) : QObject(parent)
{
    QString host = "ptt.cc";
    connect(&t, SIGNAL(receivedBytes(QByteArray)),
            this, SLOT(dumpByteArray(QByteArray)));
    t.connectTo(host);
}

void TelnetTester::dumpByteArray(QByteArray array)
{
    qDebug() << array;
}
