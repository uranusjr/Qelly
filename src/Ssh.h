#ifndef SSH_H
#define SSH_H

#include "AbstractConnection.h"
class QProcess;

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

private slots:
    void onProcessStarted();
    void onProcessReadyRead();
    void onProcessError();
    void onProcessFinished();

private:
    QProcess *_socket;
    bool _isBbs;
};

}   // namespace Connection

}   // namespace UJ

#endif // SSH_H
