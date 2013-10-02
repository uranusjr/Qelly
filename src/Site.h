/*****************************************************************************
 * Site.h
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

#ifndef SITE_H
#define SITE_H

#include <QObject>
#include "Globals.h"

namespace UJ
{

namespace Connection
{

class SitePrivate;

class Site : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Site)
    SitePrivate *d_ptr;

public:
    explicit Site(const QString &form = "protocol://site.address",
                  const QString &name = "Name", QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);
    QString address() const;
    void setAddress(const QString &address);
    Type type() const;
    qint16 port() const;
    BBS::Encoding encoding() const;
    void setEncoding(BBS::Encoding encoding);
    BBS::AnsiColorKey colorKey() const;
    void setColorKey(BBS::AnsiColorKey k);
    bool manualDoubleByte() const;
    void setManualDoubleByte(bool db);
};

}       // namespace Connection

}       // namespace UJ

#endif // SITE_H
