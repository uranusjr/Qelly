#include "Site.h"

namespace UJ
{

namespace Connection
{

Site::Site(QString address, QString name, QObject *parent) : QObject(parent)
{
    setName(name);
    setAddress(address);
    setEncoding(BBS::EncodingUnKnown);
    setColorKey(BBS::ColorKeyCtrlU);
    setManualDoubleByte(false);
}

}   // namespace Connection

}   // namespace UJ
