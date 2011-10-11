#include "Site.h"
#include "SharedPreferences.h"

namespace UJ
{

namespace Connection
{

Site::Site(QString address, QString name, QObject *parent) : QObject(parent)
{
    setName(name);
    setAddress(address);
    Qelly::SharedPreferences *prefs =
            Qelly::SharedPreferences::sharedInstance();
    setEncoding(prefs->defaultEncoding());
    setColorKey(prefs->defaultColorKey());
    setManualDoubleByte(prefs->manualDoubleByte());
}

}   // namespace Connection

}   // namespace UJ
