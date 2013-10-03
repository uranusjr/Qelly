/*****************************************************************************
 * Ssh.cpp
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

#include "Ssh.h"
#include <QProcess>
#include "SharedPreferences.h"
#include "Site.h"

namespace UJ
{

namespace Connection
{

Ssh::Ssh(QObject *parent) : AbstractConnection(parent)
{
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
    close();
}

bool Ssh::connectTo(const QString &address, qint16 port)
{
    setProcessing(true);

    _socket->setReadChannel(QProcess::StandardOutput);
    _socket->setReadChannelMode(QProcess::MergedChannels);

    port = port < 0 ? DefaultPort : port;
    QStringList args;

#if defined Q_OS_WIN32
    // Plink args
    args << "-t"
         << "-x"
         << "-P" << QString::number(port)
         << address;
#elif defined Q_OS_UNIX
    // OpenSSH args
    args << "-tt"
             << "-e" << "none"  // Do not use EscapeChar
             << "-x"
             << "-p" << QString::number(port)
             << address;
#elif defined Q_OS_LINUX
    // Use OpenSSH (same as Q_OS_UNIX)
    args << "-tt"
             << "-e" << "none"  // Do not use EscapeChar
             << "-x"
             << "-p" << QString::number(port)
             << address;
#endif

    _socket->start(Qelly::SharedPreferences::sharedInstance()->sshClientPath(),
                   args);

    return true;
}

void Ssh::close()
{
    _socket->close();
}

void Ssh::reconnect()
{
    close();
    connectToSite(site());
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

    setLastTouch();
    _socket->write(bytes);
}

}   // namespace Connection

}   // namespace UJ
