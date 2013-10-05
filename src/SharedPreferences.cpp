/*****************************************************************************
 * SharedPreferences.cpp
 *
 * Created: 11/10 2011 by uranusjr
 *
 * Copyright 2011 uranusjr. All rights reserved.
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

#include "SharedPreferences.h"
#include <QColor>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QSettings>
#include "Site.h"
#include "Ssh.h"

namespace UJ
{

namespace Qelly
{

SharedPreferences::SharedPreferences(QObject *parent) : QObject(parent)
{
    _settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              "uranusjr.org", "qelly", this);
}

SharedPreferences *SharedPreferences::sharedInstance()
{
    static SharedPreferences *g = new SharedPreferences();
    return g;
}

void SharedPreferences::sync()
{
    _settings->sync();
}

QByteArray SharedPreferences::windowGeometry() const
{
    return _settings->value("window frame", QByteArray()).toByteArray();
}

void SharedPreferences::setWindowGeometry(const QByteArray &geo)
{
    _settings->setValue("window frame", geo);
}

int SharedPreferences::cellWidth() const
{
    return _settings->value("cell width", 12).toInt();
}

void SharedPreferences::setCellWidth(int w)
{
    _settings->setValue("cell width", w);
}

int SharedPreferences::cellHeight() const
{
    return _settings->value("cell height", 25).toInt();
}

void SharedPreferences::setCellHeight(int h)
{
    _settings->setValue("cell height", h);
}

bool SharedPreferences::showHiddenText() const
{
    return _settings->value("show hidden text", false).toBool();
}

void SharedPreferences::setShowHiddenText(bool show)
{
    _settings->setValue("show hidden text", show);
    emit showHiddenTextChanged(show);
}

bool SharedPreferences::manualDoubleByte() const
{
    return _settings->value("manual double byte", false).toBool();
}

void SharedPreferences::setManualDoubleByte(bool enable)
{
    _settings->setValue("manual double byte", enable);
}

bool SharedPreferences::useSystemBeep() const
{
    return _settings->value("use system beep", false).toBool();
}

void SharedPreferences::setUseSystemBeep(bool use)
{
    _settings->setValue("use system beep", use);
}

QString SharedPreferences::customBeepFile() const
{
    return _settings->value("custom beep file", QString()).toString();
}

void SharedPreferences::setCustomBeepFile(const QString &filename)
{
    _settings->setValue("custom beep file", filename);
}

BBS::Encoding UJ::Qelly::SharedPreferences::defaultEncoding() const
{
    return BBS::Encoding(_settings->value("dafault encoding",
                                          int(BBS::EncodingBig5)).toInt());
}

void SharedPreferences::setDefaultEncoding(BBS::Encoding encoding)
{
    _settings->setValue("default encoding", static_cast<int>(encoding));
}

BBS::AnsiColorKey SharedPreferences::defaultColorKey() const
{
    return BBS::AnsiColorKey(_settings->value("default color key",
                                              int(BBS::ColorKeyCtrlU))
                             .toInt());
}

void SharedPreferences::seDefaultColorKey(BBS::AnsiColorKey key)
{
    _settings->setValue("default color key", static_cast<int>(key));
}

QFont SharedPreferences::defaultFont() const
{
    QFont font;
    QString name = _settings->value("default font", QString()).toString();
    bool ok = false;
    if (!name.isEmpty())
        ok = font.fromString(name);
    if (!ok)
        font = QFont("monospace", 16);
    font.setStyleHint(QFont::TypeWriter);
    return font;
}

void SharedPreferences::setDefaultFont(const QFont &font)
{
    _settings->setValue("default font", font.toString());
}

QFont SharedPreferences::doubleByteFont() const
{
    QFont font;
    QString name = _settings->value("double byte font", QString())
            .toString();
    bool ok = false;
    if (!name.isEmpty())
        ok = font.fromString(name);
    if (!ok)
        font = QFont("sans-sarif", 18);
    return font;
}

void SharedPreferences::setDoubleByteFont(const QFont &font)
{
    _settings->setValue("double byte font", font.toString());
}

int SharedPreferences::defaultFontPaddingLeft() const
{
    return _settings->value("default font padding left", 0).toInt();
}

void SharedPreferences::setDefaultFontPaddingLeft(int padding)
{
    _settings->setValue("default font padding left", padding);
}

int SharedPreferences::defaultFontPaddingBottom() const
{
    return _settings->value("default font padding bottom", 6).toInt();
}

void SharedPreferences::setDefaultFontPaddingBottom(int padding)
{
    _settings->setValue("default font padding bottom", padding);
}

int SharedPreferences::doubleByteFontPaddingLeft() const
{
    return _settings->value("double byte font padding left", 0).toInt();
}

void SharedPreferences::setDoubleByteFontPaddingLeft(int padding)
{
    _settings->setValue("double byte font padding left", padding);
}

int SharedPreferences::doubleByteFontPaddingBottom() const
{
    return _settings->value("double byte font padding bottom", 3).toInt();
}

void SharedPreferences::setDoubleByteFontPaddingBottom(int padding)
{
    _settings->setValue("double byte font padding bottom", padding);
}

QColor SharedPreferences::colorBlack() const
{
    return _settings->value("color black", QColor(Qt::black)).value<QColor>();
}

void SharedPreferences::setColorBlack(const QColor &color)
{
    _settings->setValue("color black", color);
}

QColor SharedPreferences::colorBlackBright() const
{
    return _settings->value("color black bright",
                            QColor(Qt::darkGray)).value<QColor>();
}

void SharedPreferences::setColorBlackBright(const QColor &color)
{
    _settings->setValue("color black bright", color);
}

QColor SharedPreferences::colorRed() const
{
    return _settings->value("color red", QColor(Qt::darkRed)).value<QColor>();
}

void SharedPreferences::setColorRed(const QColor &color)
{
    _settings->setValue("color red", color);
}

QColor SharedPreferences::colorRedBright() const
{
    return _settings->value("color red bright",
                            QColor(Qt::red)).value<QColor>();
}

void SharedPreferences::setColorRedBright(const QColor &color)
{
    _settings->setValue("color red bright", color);
}

QColor SharedPreferences::colorGreen() const
{
    return _settings->value("color green",
                            QColor(Qt::darkGreen)).value<QColor>();
}

void SharedPreferences::setColorGreen(const QColor &color)
{
    _settings->setValue("color green", color);
}

QColor SharedPreferences::colorGreenBright() const
{
    return _settings->value("color green bright",
                            QColor(Qt::green)).value<QColor>();
}

void SharedPreferences::setColorGreenBright(const QColor &color)
{
    _settings->setValue("color green bright", color);
}

QColor SharedPreferences::colorYellow() const
{
    return _settings->value("color yellow",
                            QColor(Qt::darkYellow)).value<QColor>();
}

void SharedPreferences::setColorYellow(const QColor &color)
{
    _settings->setValue("color yellow", color);
}

QColor SharedPreferences::colorYellowBright() const
{
    return _settings->value("color yellow bright",
                            QColor(Qt::yellow)).value<QColor>();
}

void SharedPreferences::setColorYellowBright(const QColor &color)
{
    _settings->setValue("color yellow bright", color);
}

QColor SharedPreferences::colorBlue() const
{
    return _settings->value("color blue", QColor(Qt::darkBlue)).value<QColor>();
}

void SharedPreferences::setColorBlue(const QColor &color)
{
    _settings->setValue("color blue", color);
}

QColor SharedPreferences::colorBlueBright() const
{
    return _settings->value("color blue bright",
                            QColor(Qt::blue)).value<QColor>();
}

void SharedPreferences::setColorBlueBright(const QColor &color)
{
    _settings->setValue("color blue bright", color);
}

QColor SharedPreferences::colorMagenta() const
{
    return _settings->value("color magenta",
                            QColor(Qt::darkMagenta)).value<QColor>();
}

void SharedPreferences::setColorMagenta(const QColor &color)
{
    _settings->setValue("color magenta", color);
}

QColor SharedPreferences::colorMagentaBright() const
{
    return _settings->value("color magenta bright",
                            QColor(Qt::magenta)).value<QColor>();
}

void SharedPreferences::setColorMagentaBright(const QColor &color)
{
    _settings->setValue("color magenta bright", color);
}

QColor SharedPreferences::colorCyan() const
{
    return _settings->value("color cyan", QColor(Qt::darkCyan)).value<QColor>();
}

void SharedPreferences::setColorCyan(const QColor &color)
{
    _settings->setValue("color cyan", color);
}

QColor SharedPreferences::colorCyanBright() const
{
    return _settings->value("color cyan bright",
                            QColor(Qt::cyan)).value<QColor>();
}

void SharedPreferences::setColorCyanBright(const QColor &color)
{
    _settings->setValue("color cyan bright", color);
}

QColor SharedPreferences::colorWhite() const
{
    return _settings->value("color white", QColor(Qt::gray)).value<QColor>();
}

void SharedPreferences::setColorWhite(const QColor &color)
{
    _settings->setValue("color white", color);
}

QColor SharedPreferences::colorWhiteBright() const
{
    return _settings->value("color white bright",
                            QColor(Qt::white)).value<QColor>();
}

void SharedPreferences::setColorWhiteBright(const QColor &color)
{
    _settings->setValue("color white bright", color);
}

void SharedPreferences::setColor(const QColor &c, int index, bool bright)
{
    switch (index)
    {
    case 0:
        return bright ? setColorBlackBright(c) : setColorBlack(c);
        break;
    case 1:
        return bright ? setColorRedBright(c) : setColorRed(c);
        break;
    case 2:
        return bright ? setColorGreenBright(c) : setColorGreen(c);
        break;
    case 3:
        return bright ? setColorYellowBright(c) : setColorYellow(c);
        break;
    case 4:
        return bright ? setColorBlueBright(c) : setColorBlue(c);
        break;
    case 5:
        return bright ? setColorMagentaBright(c) : setColorMagenta(c);
        break;
    case 6:
        return bright ? setColorCyanBright(c) : setColorCyan(c);
        break;
    case 7:
        return bright ? setColorWhiteBright(c) : setColorWhite(c);
        break;
    default:
        break;
    }
}

QColor SharedPreferences::fColor(int index, bool bright) const
{
    switch (index)
    {
    case 0:
        return bright ? colorBlackBright() : colorBlack();
    case 1:
        return bright ? colorRedBright() : colorRed();
    case 2:
        return bright ? colorGreenBright() : colorGreen();
    case 3:
        return bright ? colorYellowBright() : colorYellow();
    case 4:
        return bright ? colorBlueBright() : colorBlue();
    case 5:
        return bright ? colorMagentaBright() : colorMagenta();
    case 6:
        return bright ? colorCyanBright() : colorCyan();
    case 7:
        return bright ? colorWhiteBright() : colorWhite();
    default:
        return backgroundColor();
    }
}

QColor SharedPreferences::bColor(int index, bool bright) const
{
    switch (index)
    {
    case 0:
        return bright ? colorBlackBright() : colorBlack();
    case 1:
        return bright ? colorRedBright() : colorRed();
    case 2:
        return bright ? colorGreenBright() : colorGreen();
    case 3:
        return bright ? colorYellowBright() : colorYellow();
    case 4:
        return bright ? colorBlueBright() : colorBlue();
    case 5:
        return bright ? colorMagentaBright() : colorMagenta();
    case 6:
        return bright ? colorCyanBright() : colorCyan();
    case 7:
        return bright ? colorWhiteBright() : colorWhite();
    default:
        return backgroundColor();
    }
}

QColor SharedPreferences::backgroundColor() const
{
    return _settings->value("background color",
                            QColor(Qt::black)).value<QColor>();
}

void SharedPreferences::setBackgroundColor(const QColor &color)
{
    _settings->setValue("background color", color);
}

QString SharedPreferences::sshClientPath() const
{
#ifdef Q_OS_UNIX
    QString defaultPath = "/usr/bin/ssh";
#else
    QString defaultPath = "Plink.exe";
#endif
    return _settings->value("ssh client path", defaultPath).toString();
}

void SharedPreferences::setSshClientPath(const QString &path)
{
    QFileInfo info = QFileInfo(path);
    if (info.exists() && info.isExecutable())
    {
        _settings->setValue("ssh client path", path);
    }
    else
    {
        throw "Invalid SSH Client Path";
    }
}

bool SharedPreferences::isAntiIdleActive() const
{
    return _settings->value("anti idle", true).toBool();
}

void SharedPreferences::setAntiIdleActive(bool value)
{
    _settings->setValue("anti idle", value);
    emit antiIdleChanged(value);
}

bool SharedPreferences::restoreConnectionsOnStartup() const
{
    return _settings->value("restore connections on startup", true).toBool();
}

void SharedPreferences::setRestoreConnectionsOnStartup(bool value)
{
    _settings->setValue("restore connections on startup", value);
}

QList<Connection::Site *> SharedPreferences::storedConnections() const
{
    using namespace Connection;
    QVariantList values = _settings->value("stored connections").toList();

    QList<Site *> sites;
    foreach (const QVariant &value, values)
        sites << Site::fromProperties(value.toMap(), 0);
    return sites;
}

void SharedPreferences::storeConnections(const QList<Connection::Site *> &sites)
{
    QVariantList values;
    foreach (Connection::Site *site, sites)
        values << site->toPropertyMap();
    _settings->setValue("stored connections", values);
}

bool SharedPreferences::warnOnClose()
{
    return _settings->value("warn on close", true).toBool();
}

void SharedPreferences::setWarnOnClose(bool value)
{
    _settings->setValue("warn on close", value);
}

}   // namespace Qelly

}   // namespace UJ
