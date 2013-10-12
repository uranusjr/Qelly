/*****************************************************************************
 * AbstractConnection.h
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

#ifndef ABSTRACTCONNECTION_H
#define ABSTRACTCONNECTION_H

#include <QObject>
#include <QDateTime>

namespace UJ
{

namespace Connection
{

class Site;
class AbstractConnectionPrivate;

class AbstractConnection : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractConnection)
    AbstractConnectionPrivate * const d_ptr;

public:
    explicit AbstractConnection(QObject *parent = 0);
    virtual ~AbstractConnection();
    bool connectToSite(Site *site);
    static const qint16 DefaultPort = -1;

public slots:
    virtual void close() = 0;
    virtual void reconnect() = 0;
    virtual void sendBytes(QByteArray bytes) = 0;

protected:
    virtual bool connectTo(const QString &address, qint16 port) = 0;

signals:
    void connected();
    void disconnected();
    void processedBytes(QByteArray bytes);

public: // Getters & Setters
    virtual Site *site();
    virtual void setSite(Site *site);
    virtual QString name();
    virtual void setName(const QString &name);
    virtual QString address();
    virtual void setAddress(const QString &address);
    virtual bool isConnected();
    virtual void setConnected(bool isConnected);
    virtual bool isProcessing();
    virtual void setProcessing(bool isProcessing);
    virtual QDateTime lastTouch() const;
    virtual void setLastTouch(const QDateTime &dt = QDateTime());
};

}   // namespace Connection

}   // namespace UJ

#endif // ABSTRACTCONNECTION_H
