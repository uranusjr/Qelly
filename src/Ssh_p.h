/*****************************************************************************
 * Ssh_p.h
 *
 * Created: 8/10/2013 2013 by uranusjr
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

#ifndef SSH_P_H
#define SSH_P_H

#include <QPointer>
#include <QxtSshClient>

namespace UJ
{

namespace Connection
{

class Ssh;

class SshPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(Ssh)
    Ssh * const q_ptr;

public:
    SshPrivate(Ssh *q);
    bool connectTo(const QString &user, const QString &url, qint16 port);

    QPointer<QxtSshClient> client;
    QPointer<QxtSshProcess> process;
    struct HostInfo
    {
        HostInfo() : port(-1) {}
        HostInfo(const QString &u, const QString &h, qint16 p) :
            username(u), url(h), port(p) {}
        QString username;
        QString url;
        qint16 port;
    } current;

public slots:
    void sendBytes(const QByteArray &bytes);
    void close();

private slots:
    void acquireAuth(QList<QxtSshClient::AuthenticationMethod>);
    void didConnect();
    void didDisconnect();
    void didError(QxtSshClient::Error error);
    void didStart();
    void didFinish(int exitCode);
    void readData();
};

}   // namespace Connection

}   // namespace UJ

#endif // SSH_P_H
