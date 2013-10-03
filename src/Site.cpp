/*****************************************************************************
 * Site.cpp
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

#include "Site.h"
#include "AbstractConnection.h"
#include "SharedPreferences.h"

namespace UJ
{

namespace Connection
{

class SitePrivate
{
    Q_DECLARE_PUBLIC(Site)
    Site *q_ptr;

public:
    SitePrivate(Site *q, const QString &form, const QString &siteName) :
        q_ptr(q), type(TypeTelnet), name(siteName),
        port(AbstractConnection::DefaultPort)
    {
        setFullForm(form);
        Qelly::SharedPreferences *prefs =
                Qelly::SharedPreferences::sharedInstance();
        encoding = prefs->defaultEncoding();
        colorKey = prefs->defaultColorKey();
        manualDoubleByte = prefs->manualDoubleByte();
    }
    void setFullForm(QString form)
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
        //                  Customary default ports are used by specific
        //                  protocols

        fullForm = form;
        QStringList comps;

        // protocol part
        comps = form.split("://");
        if (comps.size() > 1)
        {
            if (comps.first() == "ssh")
                type = TypeSsh;
            form = comps.last();
        }

        // port part
        comps = form.split(':');
        if (comps.size() > 1)
        {
            form = comps.first();
            port = comps.last().toLong();
        }
        address = form;
    }

    Type type;
    QString fullForm;
    QString name;
    QString address;
    qint16 port;
    BBS::Encoding encoding;
    BBS::AnsiColorKey colorKey;
    bool manualDoubleByte;
};

Site::Site(const QString &form, const QString &name, QObject *parent) :
    QObject(parent), d_ptr(new SitePrivate(this, form, name))
{
}

Site *Site::fromProperties(const QMap<QString, QVariant> &properties,
                           QObject *parent)
{
    Site *site = new Site(properties["full_form"].toString(),
                          properties["name"].toString(), parent);
    for (QMap<QString, QVariant>::const_iterator i = properties.constBegin();
         i != properties.constEnd(); i++)
    {
        const char *name = i.key().toUtf8().data();
        if (site->metaObject()->indexOfProperty(name) >= 0)
            site->setProperty(name, i.value());
    }
    return site;
}

QStringList Site::propertyKeys()
{
    static QStringList keys = (QStringList() << "name" << "fullForm" <<
                               "encoding" << "colorKey" << "doubleByte");
    return keys;
}

QString Site::name() const
{
    return d_ptr->name;
}

void Site::setName(const QString &name)
{
    d_ptr->name = name;
}

void Site::setFullForm(const QString &form)
{
    d_ptr->setFullForm(form);
}

QString Site::url() const
{
    return d_ptr->address;
}
Type Site::type() const
{
    return d_ptr->type;
}

qint16 Site::port() const
{
    return d_ptr->port;
}

QString Site::fullForm() const
{
    return d_ptr->fullForm;
}

BBS::Encoding Site::encoding() const
{
    return d_ptr->encoding;
}

void Site::setEncoding(BBS::Encoding encoding)
{
    d_ptr->encoding = encoding;
}

BBS::AnsiColorKey Site::colorKey() const
{
    return d_ptr->colorKey;
}

void Site::setColorKey(BBS::AnsiColorKey k)
{
    d_ptr->colorKey = k;
}

bool Site::manualDoubleByte() const
{
    return d_ptr->manualDoubleByte;
}

void Site::setManualDoubleByte(bool db)
{
    d_ptr->manualDoubleByte = db;
}

}   // namespace Connection

}   // namespace UJ
