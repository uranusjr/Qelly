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

AbstractConnection::AbstractConnection(QObject *parent) : QObject(parent)
{
    _site = 0;
    setProcessing(false);
    setConnected(false);
}

void AbstractConnection::setSite(Site *site)
{
    if (_site)
        delete _site;
    _site = site;
    _site->setParent(this);
}

bool AbstractConnection::connectTo(Site *site)
{
    QString address = site->address();
    return connectTo(address, site->port());
}

}   // namespace Connection

}   // namespace UJ
