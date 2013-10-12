/*****************************************************************************
 * AbstractConnection.cpp
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

#include "AbstractConnection.h"
#include "Site.h"

namespace UJ
{

namespace Connection
{

class AbstractConnectionPrivate
{
    Q_DECLARE_PUBLIC(AbstractConnection)
    AbstractConnection * const q_ptr;

public:
    AbstractConnectionPrivate(AbstractConnection *q) :
        q_ptr(q), site(0) {}

    void setSite(Site *s)
    {
        if (site == s)
            return;
        if (site)
            site->deleteLater();
        site = s;
        site->setParent(q_ptr);
    }

    Site *site;
    QString name;
    QString address;
    // NOTE: Need an image member for tab icon...
    //       Haven't decided which class, maybe QIcon?
    bool isConnected;
    bool isProcessing;
    QDateTime lastTouch;
};

AbstractConnection::AbstractConnection(QObject *parent) :
    QObject(parent), d_ptr(new AbstractConnectionPrivate(this))
{
    setLastTouch();
    setProcessing(false);
    setConnected(false);
}

AbstractConnection::~AbstractConnection()
{
    delete d_ptr;
}

bool AbstractConnection::connectToSite(Site *site)
{
    setSite(site);
    QString address = site->url();
    return connectTo(address, site->port());
}

Site *AbstractConnection::site()
{
    return d_ptr->site;
}

void AbstractConnection::setSite(Site *site)
{
    d_ptr->setSite(site);
}

QString AbstractConnection::name()
{
    return d_ptr->name;
}

void AbstractConnection::setName(const QString &name)
{
    d_ptr->name = name;
}

QString AbstractConnection::address()
{
    return d_ptr->address;
}

void AbstractConnection::setAddress(const QString &address)
{
    d_ptr->address = address;
}

bool AbstractConnection::isConnected()
{
    return d_ptr->isConnected;
}

void AbstractConnection::setConnected(bool isConnected)
{
    // NOTE: Should change the icon image here...
    d_ptr->isConnected = isConnected;
}

bool AbstractConnection::isProcessing()
{
    return d_ptr->isProcessing;
}

void AbstractConnection::setProcessing(bool isProcessing)
{
    d_ptr->isProcessing = isProcessing;
}

QDateTime AbstractConnection::lastTouch() const
{
    return d_ptr->lastTouch;
}

void AbstractConnection::setLastTouch(const QDateTime &dt)
{
    Q_D(AbstractConnection);
    if (!dt.isValid())
        d->lastTouch = QDateTime::currentDateTime();
    else
        d->lastTouch = dt;
}

}   // namespace Connection

}   // namespace UJ
