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
    explicit Site(QString address = QString("protocol://site.address"),
                  QString name = QString("Name"),
                  QObject *parent = 0);

private:
    QString _name;
    QString _address;
    BBS::Encoding _encoding;
    BBS::AnsiColorKey _colorKey;
    bool _manualDoubleByte;

public:     // Setters & Getters
    inline QString name() const
    {
        return _name;
    }
    inline void setName(QString &name)
    {
        _name = name;
    }
    inline QString address() const
    {
        return _address;
    }
    inline void setAddress(QString &address)
    {
        _address = address;
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
