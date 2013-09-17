#include "Ssh.h"
#include <QProcess>
#include "SharedPreferences.h"
#include "Site.h"
#include <QDebug>

namespace UJ
{

namespace Connection
{

Ssh::Ssh(QObject *parent) : AbstractConnection(parent)
{
    _site = 0;
    _isBbs = false;
    _socket = new QProcess(this);

    connect(this, SIGNAL(receivedBytes(QByteArray)),
            this, SLOT(processBytes(QByteArray)));
    connect(_socket, SIGNAL(started()), this, SLOT(onProcessStarted()));
    connect(_socket, SIGNAL(readyReadStandardOutput()),
            this, SLOT(onProcessReadyRead()));
    connect(_socket, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(onProcessError()));
    connect(_socket, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished()));
}

Ssh::~Ssh()
{
    if (_socket->isOpen())
        _socket->close();
}

bool Ssh::connectTo(QString &address, qint16 port)
{
    setProcessing(true);

    if (!_site)
        setSite(new Site(address, address, this));

    _socket->setReadChannel(QProcess::StandardOutput);
    _socket->setReadChannelMode(QProcess::MergedChannels);

    QStringList args;
    _isBbs = address.split('@').first().contains("bbs");

    port = port < 0 ? DefaultPort : port;

    if (_isBbs)
    {
        args << "-tt"
             << "-e" << "none"  // Do not use EscapeChar
             << "-x"
             << "-p" << QString::number(port)
             << address;
    }
    else
    {
        // TODO: Customizable parameters and environment
        args << "-tt"
             << "-e" << "none"  // Do not use EscapeChar
             << "-x"
             << "-p" << QString::number(port)
             << address;

        QProcessEnvironment env;
        env.insert("TERM", "vt102");
        _socket->setProcessEnvironment(env);
    }

    _socket->start(Qelly::SharedPreferences::sharedInstance()->sshClientPath(),
                   args);

    return true;
}

void Ssh::close()
{
}

void Ssh::reconnect()
{
}

void Ssh::onProcessStarted()
{
    setConnected(true);
    setProcessing(false);
    emit connected();
}

void Ssh::onProcessReadyRead()
{
    while (_socket->bytesAvailable())
    {
        QByteArray data = _socket->read(512);
        if (data.size() > 0)
            emit receivedBytes(data);
    }
}

void Ssh::onProcessError()
{
    setProcessing(false);
}

void Ssh::onProcessFinished()
{
    setProcessing(false);
    setConnected(false);
    emit disconnected();
}

void Ssh::processBytes(QByteArray bytes)
{
    emit processedBytes(bytes);
}

void Ssh::sendBytes(QByteArray bytes)
{
    if (bytes.isEmpty())
        return;

    _socket->write(bytes);
}

}   // namespace Connection

}   // namespace UJ
