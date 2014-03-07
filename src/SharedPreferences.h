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
#include "Globals.h"
class QSettings;

namespace UJ
{

namespace Connection
{
class Site;
}

namespace Qelly
{

class SharedPreferences : public QObject
{
    Q_OBJECT

public:
    explicit SharedPreferences(QObject *parent = 0);
    static SharedPreferences *sharedInstance();

public slots:
    void sync();

signals:
    void antiIdleChanged(bool);
    void showHiddenTextChanged(bool);
    void toolbarVisibleChanged(bool);

private:
    QSettings *_settings;

public:
    // General settings
    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray &geo);
    bool isMaximized() const;
    void setMaximized(bool m);
    int cellWidth() const;
    void setCellWidth(int w);
    int cellHeight() const;
    void setCellHeight(int h);
    bool useSystemBeep() const;
    void setUseSystemBeep(bool use);
    QString customBeepFile() const;
    void setCustomBeepFile(const QString &filename);
    bool isSshEnabled() const;
    void setSshEnabled(bool value);
    QString sshClientPath() const;
    void setSshClientPath(const QString &path);
    bool isAntiIdleActive() const;
    void setAntiIdleActive(bool value);
    bool restoreConnectionsOnStartup() const;
    void setRestoreConnectionsOnStartup(bool value);
    QList<Connection::Site *> storedConnections() const;
    void storeConnections(const QList<Connection::Site *> &sites);
    bool warnOnClose();
    void setWarnOnClose(bool value);
    bool isToolbarVisible();
    void setToolbarVisible(bool value);

    // Site defaults
    BBS::Encoding defaultEncoding() const;
    void setDefaultEncoding(BBS::Encoding encoding);
    BBS::AnsiColorKey defaultColorKey() const;
    void seDefaultColorKey(BBS::AnsiColorKey key);
    bool showHiddenText() const;
    void setShowHiddenText(bool show);
    bool manualDoubleByte() const;
    void setManualDoubleByte(bool enable);

    // Fonts and cell paddings
    QFont defaultFont() const;
    void setDefaultFont(const QFont &font);
    QFont doubleByteFont() const;
    void setDoubleByteFont(const QFont &font);
    int defaultFontPaddingLeft() const;
    void setDefaultFontPaddingLeft(int padding);
    int defaultFontPaddingBottom() const;
    void setDefaultFontPaddingBottom(int padding);
    int doubleByteFontPaddingLeft() const;
    void setDoubleByteFontPaddingLeft(int padding);
    int doubleByteFontPaddingBottom() const;
    void setDoubleByteFontPaddingBottom(int padding);

    // Pasters
    QStringList activeLanguages() const;
    void setActiveLanguages(const QStringList &langs);

    // Colors
    QColor fColor(int index, bool bright = false) const;
    QColor bColor(int index, bool bright = false) const;
    void setColor(const QColor &c, int index, bool bright = false);
    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);
    QColor colorBlack() const;
    void setColorBlack(const QColor &color);
    QColor colorBlackBright() const;
    void setColorBlackBright(const QColor &color);
    QColor colorRed() const;
    void setColorRed(const QColor &color);
    QColor colorRedBright() const;
    void setColorRedBright(const QColor &color);
    QColor colorGreen() const;
    void setColorGreen(const QColor &color);
    QColor colorGreenBright() const;
    void setColorGreenBright(const QColor &color);
    QColor colorYellow() const;
    void setColorYellow(const QColor &color);
    QColor colorYellowBright() const;
    void setColorYellowBright(const QColor &color);
    QColor colorBlue() const;
    void setColorBlue(const QColor &color);
    QColor colorBlueBright() const;
    void setColorBlueBright(const QColor &color);
    QColor colorMagenta() const;
    void setColorMagenta(const QColor &color);
    QColor colorMagentaBright() const;
    void setColorMagentaBright(const QColor &color);
    QColor colorCyan() const;
    void setColorCyan(const QColor &color);
    QColor colorCyanBright() const;
    void setColorCyanBright(const QColor &color);
    QColor colorWhite() const;
    void setColorWhite(const QColor &color);
    QColor colorWhiteBright() const;
    void setColorWhiteBright(const QColor &color);
};

}   // namespace Qelly

}   // namespace UJ

#endif // SHAREDPREFERENCES_H
