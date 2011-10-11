#include "AbstractConnection.h"
#include "Site.h"
#include "Ssh.h"
#include "Telnet.h"

namespace UJ
{

namespace Connection
{

AbstractConnection::AbstractConnection(QObject *parent) : QObject(parent)
{
    _type = TypeUnKnown;
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

void AbstractConnection::dealloc()
{
    switch (_type)
    {
    case TypeTelnet:
        (static_cast<Telnet *>(this))->deleteLater();
        break;
    case TypeSsh:
        (static_cast<Ssh *>(this))->deleteLater();
        break;
    default:
        deleteLater();
        break;
    }
}

}   // namespace Connection

}   // namespace UJ
