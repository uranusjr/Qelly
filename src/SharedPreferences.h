/*****************************************************************************
 * SharedPreferences.h
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

#ifndef SHAREDPREFERENCES_H
#define SHAREDPREFERENCES_H

#include <QObject>
#include <QApplication>
#include <QColor>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QPoint>
#include <QSettings>
#include "Globals.h"
#include "Ssh.h"

namespace UJ
{

namespace Qelly
{

class SharedPreferences : public QObject
{
    Q_OBJECT

public:
    explicit SharedPreferences(QObject *parent = 0) : QObject(parent)
    {
        _settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                  "uranusjr.org", "qelly", this);
    }
    static inline SharedPreferences *sharedInstance()
    {
        static SharedPreferences *g = new SharedPreferences();
        return g;
    }

private:
    QSettings *_settings;

public: // Setters & Getters
    inline QPoint windowPosition() const
    {
        return _settings->value("window position", QPoint(200, 100)).toPoint();
    }
    inline void setWindowPosition(QPoint p)
    {
        _settings->setValue("window position", p);
    }

    inline int cellWidth() const
    {
        return _settings->value("cell width", 12).toInt();
    }
    inline void setCellWidth(int w)
    {
        _settings->setValue("cell width", w);
    }
    inline int cellHeight() const
    {
        return _settings->value("cell height", 25).toInt();
    }
    inline void setCellHeight(int h)
    {
        _settings->setValue("cell height", h);
    }
    inline bool showHiddenText() const
    {
        return _settings->value("show hidden text", false).toBool();
    }
    inline void setShowHiddenText(bool show)
    {
        _settings->setValue("show hidden text", show);
    }
    inline bool manualDoubleByte() const
    {
        return _settings->value("manual double byte", false).toBool();
    }
    inline void setManualDoubleByte(bool enable)
    {
        _settings->setValue("manual double byte", enable);
    }
    inline bool useSystemBeep() const
    {
        return _settings->value("use system beep", false).toBool();
    }
    inline void setUseSystemBeep(bool use)
    {
        _settings->setValue("use system beep", use);
    }
    inline QString customBeepFile() const
    {
        return _settings->value("custom beep file", QString()).toString();
    }
    inline void setCustomBeepFile(QString filename)
    {
        _settings->setValue("custom beep file", filename);
    }

    inline BBS::Encoding defaultEncoding() const
    {
        return BBS::Encoding(_settings->value("dafault encoding",
                                           int(BBS::EncodingBig5)).toInt());
    }
    inline void setDefaultEncoding(BBS::Encoding encoding)
    {
        _settings->setValue("default encoding", static_cast<int>(encoding));
    }
    inline BBS::AnsiColorKey defaultColorKey() const
    {
        return BBS::AnsiColorKey(_settings->value("default color key",
                                               int(BBS::ColorKeyCtrlU))
                                       .toInt());
    }
    inline void seDefaultColorKey(BBS::AnsiColorKey key)
    {
        _settings->setValue("default color key", static_cast<int>(key));
    }

    inline QFont defaultFont() const
    {
        QFont font;
        QString name = _settings->value("default font", QString()).toString();
        bool ok = font.fromString(name);
        if (!ok)
            font = QFont("Courier New", 16);
        font.setStyleHint(QFont::TypeWriter);
        return font;
    }
    inline void setDefaultFont(const QFont &font)
    {
        _settings->setValue("default font", font.toString());
    }
    inline QFont doubleByteFont() const
    {
        QFont font;
        QString name = _settings->value("double byte font", QString())
                                 .toString();
        bool ok = font.fromString(name);
        if (!ok)
            font = QFont("Microsoft JhengHei UI", 18);
        return font;
    }
    inline void setDoubleByteFont(const QFont &font)
    {
        _settings->setValue("double byte font", font.toString());
    }
    inline int defaultFontPaddingLeft() const
    {
        return _settings->value("default font padding left", 0).toInt();
    }
    inline void setDefaultFontPaddingLeft(int padding)
    {
        _settings->setValue("default font padding left", padding);
    }
    inline int defaultFontPaddingBottom() const
    {
        return _settings->value("default font padding bottom", 6).toInt();
    }
    inline void setDefaultFontPaddingBottom(int padding)
    {
        _settings->setValue("default font padding bottom", padding);
    }
    inline int doubleByteFontPaddingLeft() const
    {
        return _settings->value("double byte font padding left", 0).toInt();
    }
    inline void setDoubleByteFontPaddingLeft(int padding)
    {
        _settings->setValue("double byte font padding left", padding);
    }
    inline int doubleByteFontPaddingBottom() const
    {
        return _settings->value("double byte font padding bottom", 3).toInt();
    }
    inline void setDoubleByteFontPaddingBottom(int padding)
    {
        _settings->setValue("double byte font padding bottom", padding);
    }

    inline QColor colorBlack() const
    {
        return _settings->value("color black",
                                QColor(Qt::black)).value<QColor>();
    }
    inline void setColorBlack(QColor color)
    {
        _settings->setValue("color black", color);
    }
    inline QColor colorBlackBright() const
    {
        return _settings->value("color black bright",
                             QColor(Qt::darkGray)).value<QColor>();
    }
    inline void setColorBlackBright(QColor color)
    {
        _settings->setValue("color black bright", color);
    }
    inline QColor colorRed() const
    {
        return _settings->value("color red",
                                QColor(Qt::darkRed)).value<QColor>();
    }
    inline void setColorRed(QColor color)
    {
        _settings->setValue("color red", color);
    }
    inline QColor colorRedBright() const
    {
        return _settings->value("color red bright",
                             QColor(Qt::red)).value<QColor>();
    }
    inline void setColorRedBright(QColor color)
    {
        _settings->setValue("color red bright", color);
    }
    inline QColor colorGreen() const
    {
        return _settings->value("color green",
                             QColor(Qt::darkGreen)).value<QColor>();
    }
    inline void setColorGreen(QColor color)
    {
        _settings->setValue("color green", color);
    }
    inline QColor colorGreenBright() const
    {
        return _settings->value("color green bright",
                             QColor(Qt::green)).value<QColor>();
    }
    inline void setColorGreenBright(QColor color)
    {
        _settings->setValue("color green bright", color);
    }
    inline QColor colorYellow() const
    {
        return _settings->value("color yellow",
                             QColor(Qt::darkYellow)).value<QColor>();
    }
    inline void setColorYellow(QColor color)
    {
        _settings->setValue("color yellow", color);
    }
    inline QColor colorYellowBright() const
    {
        return _settings->value("color yellow bright",
                             QColor(Qt::yellow)).value<QColor>();
    }
    inline void setColorYellowBright(QColor color)
    {
        _settings->setValue("color yellow bright", color);
    }
    inline QColor colorBlue() const
    {
        return _settings->value("color blue",
                             QColor(Qt::darkBlue)).value<QColor>();
    }
    inline void setColorBlue(QColor color)
    {
        _settings->setValue("color blue", color);
    }
    inline QColor colorBlueBright() const
    {
        return _settings->value("color blue bright",
                             QColor(Qt::blue)).value<QColor>();
    }
    inline void setColorBlueBright(QColor color)
    {
        _settings->setValue("color blue bright", color);
    }
    inline QColor colorMagenta() const
    {
        return _settings->value("color magenta",
                             QColor(Qt::darkMagenta)).value<QColor>();
    }
    inline void setColorMagenta(QColor color)
    {
        _settings->setValue("color magenta", color);
    }
    inline QColor colorMagentaBright() const
    {
        return _settings->value("color magenta bright",
                             QColor(Qt::magenta)).value<QColor>();
    }
    inline void setColorMagentaBright(QColor color)
    {
        _settings->setValue("color magenta bright", color);
    }
    inline QColor colorCyan() const
    {
        return _settings->value("color cyan",
                             QColor(Qt::darkCyan)).value<QColor>();
    }
    inline void setColorCyan(QColor color)
    {
        _settings->setValue("color cyan", color);
    }
    inline QColor colorCyanBright() const
    {
        return _settings->value("color cyan bright",
                             QColor(Qt::cyan)).value<QColor>();
    }
    inline void setColorCyanBright(QColor color)
    {
        _settings->setValue("color cyan bright", color);
    }
    inline QColor colorWhite() const
    {
        return _settings->value("color white",
                             QColor(Qt::gray)).value<QColor>();
    }
    inline void setColorWhite(QColor color)
    {
        _settings->setValue("color white", color);
    }
    inline QColor colorWhiteBright() const
    {
        return _settings->value("color white bright",
                             QColor(Qt::white)).value<QColor>();
    }
    inline void setColorWhiteBright(QColor color)
    {
        _settings->setValue("color white bright", color);
    }

    inline void setColor(QColor c, int index, bool bright = false)
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
    inline QColor fColor(int index, bool bright = false) const
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
            return bright ? colorWhiteBright() : colorWhite();
        }
    }
    inline QColor bColor(int index, bool bright = false) const
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
            return bright ? colorBlackBright() : colorBlack();
        }
    }
    inline QColor backgroundColor() const
    {
        return _settings->value("background color",
                             QColor(Qt::black)).value<QColor>();
    }
    inline void setBackgroundColor(QColor color)
    {
        _settings->setValue("background color", color);
    }
    inline QString sshClientPath() const
    {
#ifdef Q_OS_UNIX
        QString defaultPath = "/usr/bin/ssh";
#else
        QString defaultPath = "Plink.exe";
#endif
        return _settings->value("ssh client path", defaultPath).toString();
    }
    inline void setSshClientPath(QString path)
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
};

}   // namespace Qelly

}   // namespace UJ

#endif // SHAREDPREFERENCES_H
