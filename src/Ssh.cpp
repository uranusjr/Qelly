#include "Ssh.h"
#include <QTcpSocket>
#include <QDebug>

namespace UJ
{

namespace Connection
{

Ssh::Ssh(QObject *parent) : AbstractConnection(parent)
{
    _site = 0;
    _socket = new QTcpSocket(this);
    connect(_socket, SIGNAL(hostFound()), this, SLOT(onSocketHostFound()));
    connect(_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError()));
    connect(_socket, SIGNAL(disconnected()),
            this, SLOT(onSocketDisconnected()));
}

Ssh::~Ssh()
{
}

bool Ssh::connectTo(Site *s)
{
}

bool Ssh::connectTo(QString &address, qint16 port)
{
    if (port < 0)
        return false;

    setProcessing(true);
    _socket->connectToHost(address, port);
}

void Ssh::close()
{
}

void Ssh::reconnect()
{
}

void Ssh::onSocketHostFound()
{
    qDebug() << "Found";
}

void Ssh::onSocketConnected()
{
    setConnected(true);
    setProcessing(false);
    emit connected();
    qDebug() << "Connected";
}

void Ssh::onSocketReadyRead()
{
    qDebug() << "ReadyRead";
    while (_socket->bytesAvailable())
    {
        QByteArray data = _socket->read(512);
        if (data.size() > 0)
            emit receivedBytes(data);
    }
}

void Ssh::onSocketError()
{
    qDebug() << "Error";
    setProcessing(false);
}

void Ssh::onSocketDisconnected()
{
    qDebug() << "Disconnected";
    setProcessing(false);
    setConnected(false);
    emit disconnected();
}


void Ssh::processBytes(QByteArray bytes)
{
}

void Ssh::sendBytes(QByteArray bytes)
{
}

void Ssh::sendCommand(uchar cmd, uchar option)
{
}

}   // namespace Connection

}   // namespace UJ
