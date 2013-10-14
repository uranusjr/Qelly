/*****************************************************************************
 * View_p.h
 *
 * Created: 21/9 2013 by uranusjr
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

#ifndef VIEW_P_H
#define VIEW_P_H

#include <cctype>
#include <QPixmap>
#include <QPoint>
#include <QQueue>
#include <QRect>
#include <QTextStream>
#include <QVector>
#include "Globals.h"
#include "Terminal.h"
class QMenu;
class QPainter;
class QTimer;

namespace UJ
{

namespace Connection
{
class Terminal;
}

namespace Qelly
{

class View;
class PreeditTextHolder;
class SharedPreferences;

class ViewPrivate
{
    Q_DECLARE_PUBLIC(View)

private:
    View *q_ptr;

public:
    ViewPrivate(View *q);
    ~ViewPrivate();

    void buildInfo();
    void updateCellSize();

    int indexFromPoint(const QPoint &point);
    QPoint pointFromIndex(int x, int y);
    void moveCursorTo(int destRow, int destCol);
    void selectWordAround(int r, int c);
    int characterFromKeyPress(int key, Qt::KeyboardModifiers mod, bool *ok);
    void addActionsToContextMenu(QMenu *menu);

    void handleArrowKey(int key);   // Up, Down, Left, Right
    void handleJumpKey(int key);    // PgUp, PgDn, Home, End
    void handleForwardDeleteKey();
    void handleAsciiDelete();       // 0x7f

    void displayCellAt(int column, int row);
    void drawSpecialSymbol(ushort code, int row, int column,
                           BBS::CellAttribute left, BBS::CellAttribute right);
    void drawDoubleColor(ushort code, int row, int column,
                         BBS::CellAttribute left, BBS::CellAttribute right);
    void paintSelection();
    void paintBlink(QRect &r);
    void refreshHiddenRegion();
    void clearSelection();
    void updateText(int row, int column);

    inline bool isAlphanumeric(uchar c) const;
    inline bool isSpecialSymbol(ushort code) const;

    inline QString shortUrlFromString(const QString &source) const;
    inline QString longUrlFromString(const QString &source) const;
    inline bool isUrlLike(const QString &s) const;
    inline bool hasProtocolPrefix(const QString &s) const;
    inline QString realize(const QString &url);
    void addUrlToMenu(const QString &url, QMenu *menu) const;
    QString selection() const;

    void showPreeditHolder();
    void hidePreeditHolder();
    void updateCursor(const QPoint &pos, Qt::KeyboardModifiers modifiers,
                      Qt::MouseButtons buttons);

    SharedPreferences *prefs;
    int cellWidth;
    int cellHeight;
    int row;
    int column;
    int x;
    int y;
    QQueue<uchar> insertBuffer;
    QTimer *insertTimer;
    int selectedStart;
    int selectedLength;
    QPixmap *backImage;
    bool blinkTicker;
    Connection::Terminal *terminal;
    QPainter *painter;
    QString address;
    PreeditTextHolder *preeditHolder;
};

bool ViewPrivate::isAlphanumeric(uchar c) const
{
    return (std::isalnum(c) || (c == '-') || (c == '_') || (c == '.'));
}

bool ViewPrivate::isSpecialSymbol(ushort code) const
{
    switch (code)
    {
    case 0x2581:    // ▁ Lower one eighth block
    case 0x2582:    // ▂ Lower one quarter block
    case 0x2583:    // ▃ Lower three eighths block
    case 0x2584:    // ▄ Lower half block
    case 0x2585:    // ▅ Lower five eighths block
    case 0x2586:    // ▆ Lower three quarters block
    case 0x2587:    // ▇ Lower seven eights block
    case 0x2588:    // █ Full block
    case 0x2589:    // ▉ Left seven eights block
    case 0x258a:    // ▊ Left three quarters block
    case 0x258b:    // ▋ Left five eighths block
    case 0x258c:    // ▌ Left half block
    case 0x258d:    // ▍ Left three eighths block
    case 0x258e:    // ▎ Left one quarter block
    case 0x258f:    // ▏ Left one eighth block
    case 0x25e2:    // ◢ Black lower right triangle
    case 0x25e3:    // ◣ Black lower left triangle
    case 0x25e4:    // ◤ Black upper left triangle
    case 0x25e5:    // ◥ Black upper right triangle
    case 0x25fc:    // ◼ Black medium square
        return true;
    default:
        return false;
    }
    return false;
}

QString ViewPrivate::shortUrlFromString(const QString &source) const
{
    QString result;
    QTextStream s(&result);
    foreach (const QChar &c, source)
    {
        ushort code = c.unicode();
        if (code >= '!' && code <= '~')
            s << c;
    }
    return result;
}

QString ViewPrivate::longUrlFromString(const QString &source) const
{
    // If the line is potentially a URL that is too long (contains "\\\r"),
    // try to fix it by removing "\\\r"
    return QString(source).replace("\\\r", "");
}

bool ViewPrivate::isUrlLike(const QString &s) const
{
    // A string is URL-like if it...
    // 1. contains two or more non-zero-length dot-separated components; and
    // 2. does not contain any continuous dot-dot sequence.
    int count = 0;
    foreach (const QString &component, s.split("."))
    {
        if (component.size())
            count++;
        else
            break;
        if (count > 1)
            return true;
    }
    return false;
}

bool ViewPrivate::hasProtocolPrefix(const QString &s) const
{
    foreach (const QString &p, Connection::Terminal::protocols())
    {
        if (s.startsWith(p))
            return true;
    }
    return false;
}

QString ViewPrivate::realize(const QString &url)
{
    if (hasProtocolPrefix(url))
        return url;
    return QString("http://%1").arg(url);
}

}   // namespace Qelly

}   // namespace UJ

#endif // VIEW_P_H
