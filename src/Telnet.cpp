/*****************************************************************************
 * Telnet.cpp
 *
 * Created: 05/10 2011 by uranusjr
 *
 * Copyright 2013 uranusjr. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/

#include "Telnet.h"
#include <QAbstractSocket>
#include <QHostInfo>
#include <QTcpSocket>
#include "YLTelnet.h"
#include "Site.h"

namespace UJ
{

namespace Connection
{

class TelnetPrivate
{
    Q_DECLARE_PUBLIC(Telnet)
    Telnet * const q_ptr;

public:
    TelnetPrivate(Telnet *q) :
        q_ptr(q), sbOption(TELOPT_BINARY), socket(new QTcpSocket(q)),
        port(Telnet::DefaultPort), synced(false), state(TOP_LEVEL) {}
    void processBytes(QByteArray bytes);

    QByteArray sbBuffer;
    uchar sbOption;
    QTcpSocket *socket;
    qint16 port;
    bool synced;

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
    } state;

private:
    void sendCommand(uchar cmd, uchar option);
    void sendBytes(QByteArray bytes);
    void handleStateTopLevel(uchar c, QQueue<uchar> *buffer);
    void handleStateSeenCr(uchar c, QQueue<uchar> *buffer);
    void handleStateSeenIac(uchar c);
    void handleStateSeenWill(uchar c);
    void handleStateSeenDo(uchar c);
    void handleStateSubNegIac(uchar c);
};

void TelnetPrivate::processBytes(QByteArray bytes)
{
    QQueue<uchar> buffer;
    foreach (uchar c, bytes)
    {
        switch (state)
        {
        case TOP_LEVEL:
            handleStateTopLevel(c, &buffer);
            break;
        case SEENCR:
            handleStateSeenCr(c, &buffer);
            break;
        case SEENIAC:
            handleStateSeenIac(c);
            break;
        case SEENWILL:
            handleStateSeenWill(c);
            break;
        case SEENWONT:
            sendCommand(DONT, c);
            state = TOP_LEVEL;
            break;
        case SEENDO:
            handleStateSeenDo(c);
            break;
        case SEENDONT:
            sendCommand(WONT, c);
            state = TOP_LEVEL;
            break;
        case SEENSB:
            sbOption = c;
            sbBuffer.clear();
            state = SUBNEGOT;
            break;
        case SUBNEGOT:
            if (c == IAC)
                state = SUBNEG_IAC;
            else
                sbBuffer.append(c);
            break;
        case SUBNEG_IAC:
            handleStateSubNegIac(c);
            break;
        default:
            break;
        }
    }

    int chunk_sz = 256;
    while (!buffer.isEmpty())
    {
        QByteArray data;
        int length = buffer.size() < chunk_sz ? buffer.size() : chunk_sz;
        for (int i = 0; i < length; i++)
            data.append(buffer.dequeue());
        emit q_ptr->processedBytes(data);
    }
}

void TelnetPrivate::handleStateTopLevel(uchar c, QQueue<uchar> *buffer)
{
    switch (c)
    {
    case IAC:
        state = SEENIAC;
        break;
    default:
        if (!synced)
            buffer->enqueue(c);
        else if (c == DM)
            synced = false;
        if (c == CR)
            state = SEENCR;
        else
            state = TOP_LEVEL;
        break;
    }
}

void TelnetPrivate::handleStateSeenCr(uchar c, QQueue<uchar> *buffer)
{
    switch (c)
    {
    case NUL:
        state = TOP_LEVEL;
        break;
    case IAC:
        state = SEENIAC;
        break;
    default:
        if (!synced)
            buffer->enqueue(c);
        else if (c == DM)
            synced = false;
        if (c == CR)
            state = SEENCR;
        else
            state = TOP_LEVEL;
        break;
    }
}

void TelnetPrivate::handleStateSeenIac(uchar c)
{
    switch (c)
    {
    case DO:
        state = SEENDO;
        break;
    case DONT:
        state = SEENDONT;
        break;
    case WILL:
        state = SEENWILL;
        break;
    case WONT:
        state = SEENWONT;
        break;
    case SB:
        state = SEENSB;
        break;
    case DM:
        synced = false;
        state = TOP_LEVEL;
        break;
    default:
        state = TOP_LEVEL;
        break;
    }
}

void TelnetPrivate::handleStateSeenWill(uchar c)
{
    switch (c)
    {
    case TELOPT_ECHO:
    case TELOPT_SGA:
    case TELOPT_BINARY:
        sendCommand(DO, c);
        break;
    default:
        sendCommand(DONT, c);
        break;
    }
    state = TOP_LEVEL;
}

void TelnetPrivate::handleStateSeenDo(uchar c)
{
    QByteArray bs;
    switch (c)
    {
    case TELOPT_NAWS:
        sendCommand(WILL, c);
        bs.append(IAC);
        bs.append(SB);
        bs.append(TELOPT_NAWS);
        bs.append('\x0');
        bs.append('\x50');
        bs.append('\x0');
        bs.append('\x18');
        bs.append(IAC);
        bs.append(SE);
        sendBytes(bs);
        break;
    case TELOPT_TTYPE:
    case TELOPT_BINARY:
        sendCommand(WILL, c);
        break;
    default:
        sendCommand(WONT, c);
        break;
    }
    state = TOP_LEVEL;
}

void TelnetPrivate::handleStateSubNegIac(uchar c)
{
    if (c == SE)
    {
        if (sbOption == TELOPT_TTYPE &&
                sbBuffer.size() == 1 && sbBuffer.at(0) == TELQUAL_SEND)
        {
            QByteArray bs;
            bs.append(IAC);
            bs.append(SB);
            bs.append(TELOPT_TTYPE);
            bs.append(static_cast<char>(TELQUAL_IS));
            bs.append("vt100");
            bs.append(IAC);
            bs.append(SE);
            sendBytes(bs);
        }
        state = TOP_LEVEL;
        sbBuffer.clear();
    }
    else
    {
        sbBuffer.append(c);
        state = SUBNEGOT;
    }
}

void TelnetPrivate::sendCommand(uchar cmd, uchar option)
{
    QByteArray data;
    data.append(IAC);
    data.append(cmd);
    data.append(option);
    q_ptr->sendBytes(data);
}

void TelnetPrivate::sendBytes(QByteArray bytes)
{
    if (bytes.isEmpty())
        return;

    switch (socket->state())
    {
    case QAbstractSocket::UnconnectedState:
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::ClosingState:
        return;
    default:
        break;
    }
    qint64 sz = socket->write(bytes);
    if (sz <= 0)
    {
        sendBytes(bytes);               // Failed. Retry.
    }
    else
    {
        q_ptr->setLastTouch();
        if (sz != bytes.size())
            sendBytes(bytes.mid(sz));   // restart from want we left off
    }
}

Telnet::Telnet(QObject *parent) :
    AbstractConnection(parent), d_ptr(new TelnetPrivate(this))
{
    Q_D(Telnet);
    connect(d->socket, SIGNAL(hostFound()), SLOT(onSocketHostFound()));
    connect(d->socket, SIGNAL(connected()), SLOT(onSocketConnected()));
    connect(d->socket, SIGNAL(readyRead()), SLOT(onSocketReadyRead()));
    connect(d->socket, SIGNAL(disconnected()), SLOT(onSocketDisconnected()));
    connect(d->socket, SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(onSocketError()));
}

Telnet::~Telnet()
{
    close();
}

bool Telnet::connectTo(const QString &address, qint16 port)
{
    Q_D(Telnet);
    setProcessing(true);
    d->port = port < 0 ? DefaultPort : port;
    d->socket->connectToHost(address, d->port);
    return true;
}

void Telnet::close()
{
    d_ptr->socket->close();
}

void Telnet::reconnect()
{
    close();
    connectToSite(site());
}

void Telnet::onSocketHostFound()
{
}

void Telnet::onSocketConnected()
{
    setConnected(true);
    setProcessing(false);
    emit connected();
}

void Telnet::onSocketReadyRead()
{
    Q_D(Telnet);
    while (d->socket->bytesAvailable())
    {
        QByteArray data = d->socket->read(512);
        if (data.size() > 0)
            d->processBytes(data);
    }
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

void Telnet::sendBytes(QByteArray bytes)
{
    d_ptr->sendBytes(bytes);
}

}   // namespace Connection

}   // namespace UJ
