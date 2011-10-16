#ifndef SSH_H
#define SSH_H

#include "AbstractConnection.h"
class QTcpSocket;

namespace UJ
{

namespace Connection
{

class Ssh : public AbstractConnection
{
    Q_OBJECT

public:
    explicit Ssh(QObject *parent = 0);
    virtual ~Ssh();
    virtual bool connectTo(Site *s);
    virtual bool connectTo(QString &address, qint16 port);
    static const qint16 DefaultPort = 22;

signals:
    void receivedBytes(QByteArray data);
    void hasBytesToSend(QByteArray bytes);

public slots:
    virtual void close();
    virtual void reconnect();
    virtual void processBytes(QByteArray bytes);
    virtual void sendBytes(QByteArray bytes);
    void sendCommand(uchar cmd, uchar option);

private slots:
    void onSocketHostFound();
    void onSocketConnected();
    void onSocketReadyRead();
    void onSocketError();
    void onSocketDisconnected();

private:
    QTcpSocket *_socket;
};

}   // namespace Connection

}   // namespace UJ

#endif // SSH_H
