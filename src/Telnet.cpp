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

Telnet::Telnet(QObject *parent) : AbstractConnection(parent)
{
    _site = 0;
    _state = TOP_LEVEL;
    _synced = false;
    _sbBuffer = new QByteArray();
    _socket = new QTcpSocket(this);
    connect(this, SIGNAL(receivedBytes(QByteArray)),
            this, SLOT(processBytes(QByteArray)));
    connect(this, SIGNAL(hasBytesToSend(QByteArray)),
            this, SLOT(sendBytes(QByteArray)));
    connect(_socket, SIGNAL(hostFound()), this, SLOT(onSocketHostFound()));
    connect(_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError()));
    connect(_socket, SIGNAL(disconnected()),
            this, SLOT(onSocketDisconnected()));
}

Telnet::~Telnet()
{
    delete _sbBuffer;
}

bool Telnet::connectTo(Site *s)
{
    QString address = s->address();
    return connectTo(address, DefaultPort);
}

bool Telnet::connectTo(QString &address, qint16 port)
{
    if (port < 0)
        return false;

    setProcessing(true);
    _port = port;

    if (!_site)
        setSite(new Site(address, address, this));
    _socket->connectToHost(address, _port);

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

void Telnet::onSocketConnected()
{
    setConnected(true);
    setProcessing(false);
    emit connected();
}

void Telnet::onSocketReadyRead()
{
    while (_socket->bytesAvailable())
    {
        QByteArray data = _socket->read(512);
        if (data.size() > 0)
            emit receivedBytes(data);
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

void Telnet::processBytes(QByteArray bytes)
{
    QQueue<uchar> buffer;
    for (int i = 0; i < bytes.size(); i++)
    {
        uchar c = static_cast<uchar>(bytes[i]);
        switch (_state)
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
            _state = TOP_LEVEL;
            break;
        case SEENDO:
            handleStateSeenDo(c);
            break;
        case SEENDONT:
            sendCommand(WONT, c);
            break;
        case SEENSB:
            _sbOption = c;
            _sbBuffer->clear();
            _state = SUBNEGOT;
            break;
        case SUBNEGOT:
            if (c == IAC)
                _state = SUBNEG_IAC;
            else
                _sbBuffer->append(c);
            break;
        case SUBNEG_IAC:
            handleStateSubNegIac(c);
            break;
        default:
            break;
        }
    }

    int chunk_sz = 1024;
    while (!buffer.isEmpty())
    {
        QByteArray data;
        int length = buffer.size() < chunk_sz ? buffer.size() : chunk_sz;
        for (int i = 0; i < length; i++)
            data.append(buffer.dequeue());
        emit processedBytes(data);
    }
}

void Telnet::handleStateTopLevel(uchar c, QQueue<uchar> *buffer)
{
    switch (c)
    {
    case IAC:
        _state = SEENIAC;
        break;
    default:
        if (!_synced)
            buffer->enqueue(c);
        else if (c == DM)
            _synced = false;
        if (c == CR)
            _state = SEENCR;
        else
            _state = TOP_LEVEL;
        break;
    }
}

void Telnet::handleStateSeenCr(uchar c, QQueue<uchar> *buffer)
{
    switch (c)
    {
    case NUL:
        _state = TOP_LEVEL;
        break;
    case IAC:
        _state = SEENIAC;
        break;
    default:
        if (!_synced)
            buffer->enqueue(c);
        else if (c == DM)
            _synced = false;
        if (c == CR)
            _state = SEENCR;
        else
            _state = TOP_LEVEL;
        break;
    }
}

void Telnet::handleStateSeenIac(uchar c)
{
    switch (c)
    {
    case DO:
        _state = SEENDO;
        break;
    case DONT:
        _state = SEENDONT;
        break;
    case WILL:
        _state = SEENWILL;
        break;
    case WONT:
        _state = SEENWONT;
        break;
    case SB:
        _state = SEENSB;
        break;
    case DM:
        _synced = false;
        _state = TOP_LEVEL;
        break;
    default:
        _state = TOP_LEVEL;
        break;
    }
}

void Telnet::handleStateSeenWill(uchar c)
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
    _state = TOP_LEVEL;
}

void Telnet::handleStateSeenDo(uchar c)
{
    QByteArray bs;
    switch (c)
    {
    case TELOPT_NAWS:
        sendCommand(WILL, c);
        bs.append(static_cast<char>(IAC));
        bs.append(static_cast<char>(SB));
        bs.append(static_cast<char>(TELOPT_NAWS));
        bs.append("\x0\x50\x0\x18");
        bs.append(static_cast<char>(IAC));
        bs.append(static_cast<char>(SE));
        emit hasBytesToSend(bs);
        break;
    case TELOPT_TTYPE:
    case TELOPT_BINARY:
        sendCommand(WILL, c);
        break;
    default:
        sendCommand(WONT, c);
        break;
    }
    _state = TOP_LEVEL;
}

void Telnet::handleStateSubNegIac(uchar c)
{
    if (c == SE)
    {
        if (_sbOption == TELOPT_TTYPE &&
                _sbBuffer->size() == 1 && _sbBuffer->at(0) == TELQUAL_SEND)
        {
            QByteArray bs;
            bs.append(static_cast<char>(IAC));
            bs.append(static_cast<char>(SB));
            bs.append(static_cast<char>(TELOPT_TTYPE));
            bs.append(static_cast<char>(TELQUAL_IS));
            bs.append("vt100");
            bs.append(static_cast<char>(IAC));
            bs.append(static_cast<char>(SE));
            emit hasBytesToSend(bs);
        }
        _state = TOP_LEVEL;
        _sbBuffer->clear();
    }
    else
    {
        _sbBuffer->append(c);
        _state = SUBNEGOT;
    }
}

void Telnet::sendCommand(uchar cmd, uchar option)
{
    QByteArray data;
    data.append(static_cast<char>(IAC));
    data.append(cmd);
    data.append(option);
    emit hasBytesToSend(data);
}

void Telnet::sendBytes(QByteArray bytes)
{
    if (bytes.isEmpty())
        return;
    switch (_socket->state())
    {
    case QAbstractSocket::UnconnectedState:
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::ClosingState:
        return;
    default:
        break;
    }
    qint64 sz = _socket->write(bytes);
    if (sz == bytes.size())
        return;
    else if (sz <= 0)
        emit hasBytesToSend(bytes);
    else
        emit hasBytesToSend(bytes.mid(sz)); // restart from want we left off
}

}   // namespace Connection

}   // namespace UJ
