#include "Site.h"
#include "AbstractConnection.h"
#include "SharedPreferences.h"

namespace UJ
{

namespace Connection
{

Site::Site(QString form, QString name, QObject *parent) : QObject(parent)
{
    // Accepted form:
    // [protocol://]site.address[:port]
    //
    // REQUIRED:
    //  site.address    Address of site (either IP or domain name)
    //                  May contain user/password info
    //
    // OPTIONAL:
    // protocol://      protocol specifies the connection type
    //                  telnet is used by default
    // :port            port specifies the port of service
    //                  Customary default ports are used by specific services
    setName(name);
    QStringList comps;

    comps = form.split("://");
    if (comps.size() > 1)
    {
        if (comps.first() == "ssh")
            _type = TypeSsh;
        else if (comps.first() == "telnet")
            _type = TypeTelnet;
        else
            _type = TypeTelnet;
        form = comps.last();
    }
    else
    {
        _type = TypeTelnet;
    }

    comps = form.split(':');
    if (comps.size() > 1)
    {
        form = comps.first();
        _port = comps.last().toLong();
    }
    else
    {
        _port = AbstractConnection::DefaultPort;
    }

    setAddress(form);
    Qelly::SharedPreferences *prefs =
            Qelly::SharedPreferences::sharedInstance();
    setEncoding(prefs->defaultEncoding());
    setColorKey(prefs->defaultColorKey());
    setManualDoubleByte(prefs->manualDoubleByte());
}

}   // namespace Connection

}   // namespace UJ
