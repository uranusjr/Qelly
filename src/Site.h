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

class Site : public QObject
{
    Q_OBJECT

public:
    explicit Site(QString form = QString("protocol://site.address"),
                  QString name = QString("Name"),
                  QObject *parent = 0);

private:
    Type _type;
    QString _name;
    QString _address;
    qint16 _port;
    BBS::Encoding _encoding;
    BBS::AnsiColorKey _colorKey;
    bool _manualDoubleByte;

public:     // Setters & Getters
    inline QString name() const
    {
        return _name;
    }
    inline void setName(const QString &name)
    {
        _name = name;
    }
    inline QString address() const
    {
        return _address;
    }
    inline void setAddress(const QString &address)
    {
        _address = address;
    }
    inline Type type() const
    {
        return _type;
    }
    inline qint16 port() const
    {
        return _port;
    }
    inline BBS::Encoding encoding() const
    {
        return _encoding;
    }
    inline void setEncoding(BBS::Encoding encoding)
    {
        _encoding = encoding;
    }
    inline BBS::AnsiColorKey colorKey() const
    {
        return _colorKey;
    }
    inline void setColorKey(BBS::AnsiColorKey k)
    {
        _colorKey = k;
    }
    inline bool manualDoubleByte() const
    {
        return _manualDoubleByte;
    }
    inline void setManualDoubleByte(bool db)
    {
        _manualDoubleByte = db;
    }
};

}       // namespace Connection

}       // namespace UJ

#endif // SITE_H
