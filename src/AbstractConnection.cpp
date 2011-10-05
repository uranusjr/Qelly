#include "AbstractConnection.h"
#include "Site.h"

namespace UJ
{

namespace Connection
{

AbstractConnection::AbstractConnection(QObject *parent) : QObject(parent)
{
    setProcessing(false);
    setConnected(false);
}

void AbstractConnection::setSite(Site *site)
{
    delete _site;
    _site = site;
    _site->setParent(this);
}

}   // namespace Connection

}   // namespace UJ
