#ifndef TELNET_H
#define TELNET_H

#include "AbstractConnection.h"
#include <QQueue>
class QHostAddress;
class QHostInfo;
class QTcpSocket;

namespace UJ
{

namespace Connection
{

class Telnet : public AbstractConnection
{
    Q_OBJECT

public:
    explicit Telnet(QObject *parent = 0);
    virtual ~Telnet();
    virtual bool connectTo(Site *s);
    virtual bool connectTo(QString &address, qint16 port);

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
    void handleStateSeenCr(uchar c, QQueue<uchar> *buffer);
    void handleStateSeenIac(uchar c);
    void handleStateSeenWill(uchar c);
    void handleStateSeenDo(uchar c);
    void handleStateSubNegIac(uchar c);

    QByteArray *_sbBuffer;
    uchar _sbOption;
    QTcpSocket *_socket;
    qint16 _port;
    bool _synced;

    enum State
    {
        TOP_LEVEL,
        SEENIAC,
        SEENWILL,
        SEENWONT,
        SEENDO,
        SEENDONT,
        SEENSB,
        SUBNEGOT,
        SUBNEG_IAC,
        SEENCR
    } _state;
};

}   // namespace Connection

}   // namespace UJ

#endif // TELNET_H
