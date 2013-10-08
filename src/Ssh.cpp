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
#include "Ssh_p.h"
#include <QStringList>
#include <QxtSshProcess>

namespace UJ
{

namespace Connection
{

SshPrivate::SshPrivate(Ssh *q) :
    QObject(q), q_ptr(q), client(new QxtSshClient(this))
{
    connect(client, SIGNAL(authenticationRequired(
                               QList<QxtSshClient::AuthenticationMethod>)),
            SLOT(acquireAuth(QList<QxtSshClient::AuthenticationMethod>)));
    connect(client, SIGNAL(connected()), SLOT(didConnect()));
    connect(client, SIGNAL(disconnected()), SLOT(didDisconnect()));
    connect(client, SIGNAL(error(QxtSshClient::Error)),
            SLOT(didError(QxtSshClient::Error)));
}

bool SshPrivate::connectTo(const QString &user, const QString &url, qint16 port)
{
    port = port < 0 ? Ssh::DefaultPort : port;
    current = HostInfo(user, url, port);
    client->connectToHost(user, url, port);
    return true;
}

void SshPrivate::sendBytes(const QByteArray &bytes)
{
    if (process && process->isOpen())
        process->write(bytes);
}

void SshPrivate::close()
{
    if (process)
        process->close();
    if (client)
        client->disconnectFromHost();
}

void SshPrivate::acquireAuth(QList<QxtSshClient::AuthenticationMethod>)
{
    qDebug("Auth required");
}

void SshPrivate::didConnect()
{
    process = client->openProcessChannel();
    connect(process, SIGNAL(started()), SLOT(didStart()));
    connect(process, SIGNAL(finished(int)), SLOT(didFinish(int)));
    connect(process, SIGNAL(readyRead()), SLOT(readData()));
    process->requestPty();
    process->startShell();
}

void SshPrivate::didDisconnect()
{
    qDebug("Disconnected");
}

void SshPrivate::didError(QxtSshClient::Error error)
{
    switch (error)
    {
    case QxtSshClient::HostKeyUnknownError:
        client->addKnownHost(client->hostName(), client->hostKey());
        connectTo(current.username, current.url, current.port);
        break;
    default:
        qDebug("Error %d", error);
        q_ptr->setProcessing(false);
        break;
    }
}

void SshPrivate::didStart()
{
    Q_Q(Ssh);
    q->setConnected(true);
    q->setProcessing(false);
    emit q->connected();
}

void SshPrivate::didFinish(int exitCode)
{
    qDebug("Finished %d", exitCode);
    Q_Q(Ssh);
    q->setProcessing(false);
    q->setConnected(false);
    emit q->disconnected();
}

void SshPrivate::readData()
{
    while (true)
    {
        QByteArray data = process->read(512);
        if (!data.isEmpty())
            emit q_ptr->receivedBytes(data);
        else
            break;
    }
}

Ssh::Ssh(QObject *parent) :
    AbstractConnection(parent), d_ptr(new SshPrivate(this))
{
    connect(this, SIGNAL(receivedBytes(QByteArray)),
            SLOT(processBytes(QByteArray)));
}

Ssh::~Ssh()
{
    close();
}

bool Ssh::connectTo(const QString &address, qint16 port)
{
    setProcessing(true);
    QStringList comps = address.split("@", QString::SkipEmptyParts);
    if (comps.size() != 2)
        return false;
    return d_ptr->connectTo(comps.first(), comps.last(), port);
}

void Ssh::close()
{
    d_ptr->close();
}

void Ssh::reconnect()
{
    close();
    connectToSite(site());
}

void Ssh::processBytes(QByteArray bytes)
{
    emit processedBytes(bytes);
}

void Ssh::sendBytes(QByteArray bytes)
{
    if (bytes.isEmpty())
        return;
    d_ptr->sendBytes(bytes);
    setLastTouch();
}

}   // namespace Connection

}   // namespace UJ
