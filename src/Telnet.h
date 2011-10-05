#ifndef TELNET_H
#define TELNET_H

#include "AbstractConnection.h"
#include <QTcpSocket>
#include "YLTelnet.h"

namespace UJ
{

namespace Connection
{

class Telnet : public QObject
{
    Q_OBJECT

public:
    explicit Telnet(QObject *parent = 0);

signals:

public slots:

};

}   // namespace Connection

}   // namespace UJ

#endif // TELNET_H
