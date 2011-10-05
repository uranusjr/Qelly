#ifndef TELNET_H
#define TELNET_H

#include "AbstractConnection.h"
#include "YLTelnet.h"
class QHostAddress;
class QHostInfo;
class QTcpSocket;
#include <QAbstractSocket>

namespace UJ
{

namespace Connection
{

enum
{
    PortDefaultTelnet = 23
};

class Telnet : public AbstractConnection
{
    Q_OBJECT

public:
    explicit Telnet(QObject *parent = 0);
    virtual bool connectTo(Site *s);
    virtual bool connectTo(QString &address, qint16 port = PortDefaultTelnet);

signals:
    void receivedBytes(QByteArray data);

public slots:
    virtual void close();
    virtual void reconnect();
    virtual void processBytes(QByteArray data);
    virtual void sendBytes(uchar *bytes, uint length);

private slots:
    void lookedUpHostInfo(QHostInfo info);
    void onSocketHostFound();
    void onSocketConnected();
    void onSocketReadyRead();
    void onSocketError();
    void onSocketDisconnected();

private:
    QTcpSocket *_socket;
    qint16 _port;
};

}   // namespace Connection

}   // namespace UJ

#endif // TELNET_H
