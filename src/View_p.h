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
#include <QVector>
#include "Globals.h"
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

    int indexFromPoint(const QPoint &point);
    QPoint pointFromIndex(int x, int y);
    void moveCursorTo(int destRow, int destCol);
    void selectWordAround(int r, int c);
    int characterFromKeyPress(int key, Qt::KeyboardModifiers mod, bool *ok);
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

    inline int fColorIndex(BBS::CellAttribute &attribute);
    inline int bColorIndex(BBS::CellAttribute &attribute);
    inline int fBright(BBS::CellAttribute &attribute);
    inline int bBright(BBS::CellAttribute &attribute);
    inline bool isAlphanumeric(uchar c);
    inline bool isSpecialSymbol(ushort code);

    SharedPreferences *prefs;
    double cellWidth;
    double cellHeight;
    int row;
    int column;
    int x;
    int y;
    QQueue<uchar> insertBuffer;
    QTimer *insertTimer;
    int selectedStart;
    int selectedLength;
    int markedStart;
    int markedLength;
    QPixmap *backImage;
    bool backImageFlipped;
    bool blinkTicker;
    QVector<QSize> singleAdvances;
    QVector<QSize> doubleAdvances;
    Connection::Terminal *terminal;
    QPainter *painter;
    QString address;
    PreeditTextHolder *preeditHolder;
};

int ViewPrivate::fColorIndex(BBS::CellAttribute &attribute)
{
    if (attribute.f.reversed)
        return attribute.f.bColorIndex;
    else
        return attribute.f.fColorIndex;
}

int ViewPrivate::bColorIndex(BBS::CellAttribute &attribute)
{
    if (attribute.f.reversed)
        return attribute.f.fColorIndex;
    else
        return attribute.f.bColorIndex;
}

int ViewPrivate::fBright(BBS::CellAttribute &attribute)
{
    return !attribute.f.reversed && attribute.f.bright;
}

int ViewPrivate::bBright(BBS::CellAttribute &attribute)
{
    return attribute.f.reversed && attribute.f.bright;
}

bool ViewPrivate::isAlphanumeric(uchar c)
{
    return (std::isalnum(c) || (c == '-') || (c == '_') || (c == '.'));
}

bool ViewPrivate::isSpecialSymbol(ushort code)
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

}   // namespace Qelly

}   // namespace UJ

#endif // VIEW_P_H
