#ifndef TELNETTESTER_H
#define TELNETTESTER_H

#include <QObject>
#include "../Test/UJQxTestUtilities.h"
#include "Telnet.h"
class QByteArray;

class TelnetTester : public UJ::Qx::Tester
{
    Q_OBJECT

public:
    explicit TelnetTester(QObject *parent = 0);

signals:

public slots:
    void dumpByteArray(QByteArray array);

private:
    UJ::Connection::Telnet t;
};

#endif // TELNETTESTER_H
