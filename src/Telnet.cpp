#include "Telnet.h"
#include <QAbstractSocket>
#include <QHostInfo>
#include <QTcpSocket>
#include <QQueue>

namespace UJ
{

namespace Connection
{

Telnet::Telnet(QObject *parent) : AbstractConnection(parent)
{
    _socket = new QTcpSocket(this);
    connect(this, SIGNAL(receivedBytes(QByteArray)),
            this, SLOT(processBytes(QByteArray)));
    connect(_socket, SIGNAL(hostFound()), this, SLOT(onSocketHostFound()));
    connect(_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError()));
    connect(_socket, SIGNAL(disconnected()),
            this, SLOT(onSocketDisconnected()));
}

bool Telnet::connectTo(Site *s)
{
}

bool Telnet::connectTo(QString &address, qint16 port)
{
    if (port < 0)
        return false;

    setProcessing(true);
    _port = port;
    QHostInfo::lookupHost(address, this, SLOT(lookedUpHostInfo(QHostInfo)));

    return true;
}

void Telnet::close()
{
}

void Telnet::reconnect()
{
}

void Telnet::onSocketHostFound()
{
}

void Telnet::lookedUpHostInfo(QHostInfo host)
{
    if (host.error() != QHostInfo::NoError)
    {
        setProcessing(false);
        return;
    }

    QHostAddress address = host.addresses().first();
    _socket->connectToHost(address, _port);
}

void Telnet::onSocketConnected()
{
    setConnected(true);
    setProcessing(false);
    emit connected();
}

void Telnet::onSocketReadyRead()
{
    QByteArray data;
    while (_socket->bytesAvailable())
        data += _socket->readAll();
    if (data.size() > 0)
        emit receivedBytes(data);
}

void Telnet::onSocketError()
{
    setProcessing(false);
}

void Telnet::onSocketDisconnected()
{
    setProcessing(false);
    setConnected(false);
    emit disconnected();
}

void Telnet::processBytes(QByteArray data)
{

}

void Telnet::sendBytes(uchar *bytes, uint length)
{
}

}   // namespace Connection

}   // namespace UJ
