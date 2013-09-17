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
