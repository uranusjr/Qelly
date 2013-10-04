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

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString fullForm READ fullForm WRITE setFullForm)
    Q_PROPERTY(UJ::BBS::Encoding encoding READ encoding WRITE setEncoding)
    Q_PROPERTY(UJ::BBS::AnsiColorKey colorKey READ colorKey WRITE setColorKey)
    Q_PROPERTY(bool doubleByte READ manualDoubleByte WRITE setManualDoubleByte)

public:
    explicit Site(const QString &form = "protocol://site.address",
                  const QString &name = "Name", QObject *parent = 0);
    static Site *fromProperties(const QMap<QString, QVariant> &properties,
                                QObject *parent = 0);
    QVariantMap toPropertyMap() const;
    static QStringList propertyKeys();


    QString name() const;
    QString fullForm() const;
    BBS::Encoding encoding() const;
    BBS::AnsiColorKey colorKey() const;
    bool manualDoubleByte() const;

    QString url() const;
    Type type() const;
    qint16 port() const;

public slots:
    void setName(const QString &name);
    void setFullForm(const QString &form);
    void setEncoding(BBS::Encoding encoding);
    void setColorKey(BBS::AnsiColorKey k);
    void setManualDoubleByte(bool db);
};

}       // namespace Connection

}       // namespace UJ

#endif // SITE_H
