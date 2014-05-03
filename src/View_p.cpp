/*****************************************************************************
 * View_p.cpp
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

#include "View_p.h"
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QRegExp>
#include <QTimer>
#include "AbstractConnection.h"
#include "Encodings.h"
#include "PreeditTextHolder.h"
#include "SharedPreferences.h"
#include "Site.h"
#include "View.h"

namespace UJ
{

namespace Qelly
{

ViewPrivate::ViewPrivate(View *q)
    : q_ptr(q), selectedStart(PositionNotFound), selectedLength(0),
      backImage(0), blinkTicker(false), terminal(0)
{
    prefs = SharedPreferences::sharedInstance();
    painter = new QPainter();
    preeditHolder = new PreeditTextHolder(q_ptr);
    q->setFocusProxy(preeditHolder);
    preeditHolder->installEventFilter(q);
    q->connect(preeditHolder, SIGNAL(hasCommitString(QInputMethodEvent*)),
               SLOT(commitPreeditHolder(QInputMethodEvent*)));

    buildInfo();
}

ViewPrivate::~ViewPrivate()
{
    if (painter)
        delete painter;
    if (backImage)
        delete backImage;
    if (terminal)
        terminal->deleteLater();
}

void ViewPrivate::buildInfo()
{
    updateCellSize();
    insertBuffer.clear();
    insertTimer = new QTimer(q_ptr);
    q_ptr->connect(insertTimer, SIGNAL(timeout()), SLOT(popInsertBuffer()));
    // NOTE: Set _textField hidden...This is the MarkedTextView thingy
}

void ViewPrivate::updateCellSize()
{
    row = BBS::SizeRowCount;
    column = BBS::SizeColumnCount;
    cellWidth = prefs->cellWidth();
    cellHeight = prefs->cellHeight();

    if (backImage)
        delete backImage;
    backImage = new QPixmap(cellWidth * column, cellHeight * row);
    q_ptr->setFixedSize(cellWidth * column, cellHeight * row);
}

int ViewPrivate::indexFromPoint(const QPoint &point)
{
    QPoint p(point);
    if (p.rx() >= column * cellWidth)
        p.setX(column * cellWidth - 1);
    else if (p.rx() < 0)
        p.setX(0);
    if (p.ry() >= row * cellHeight)
        p.setY(row * cellHeight - 1);
    else if (p.ry() < 0)
        p.setY(0);
    int x = p.rx() / cellWidth;
    int y = p.ry() / cellHeight;

    return y * column + x;
}

QPoint ViewPrivate::pointFromIndex(int x, int y)
{
    if (x < 0)
        x = 0;
    else if (x > column)
        x = column;

    if (y < 0)
        y = 0;
    else if (y > row)
        y = row;

    return QPoint(x * cellWidth, y * cellHeight);
}

void ViewPrivate::moveCursorTo(int destRow, int destCol)
{
    QByteArray cmd;
    bool needsVertical = false;
    if (destRow > terminal->cursorRow())
    {
        needsVertical = true;
        cmd.append('\x01');
        for (int i = terminal->cursorRow(); i < destRow; i++)
            cmd.append("\x1b\x4f\x42");
    }
    else if (destRow < terminal->cursorRow())
    {
        needsVertical = true;
        cmd.append('\x01');
        for (int i = terminal->cursorRow(); i > destRow; i--)
            cmd.append("\x1b\x4f\x41");
    }

    BBS::Cell *row = terminal->cellsAtRow(destRow);
    bool siteDblByte = terminal->connection()->site()->manualDoubleByte();
    if (needsVertical)
    {
        for (int i = 0; i < destCol; i++)
        {
            if (row[i].attr.f.doubleByte != 2 || siteDblByte)
                cmd.append("\x1b\x4f\x43");
        }
    }
    else if (destCol > terminal->cursorColumn())
    {
        for (int i = terminal->cursorColumn(); i < destCol; i++)
        {
            if (row[i].attr.f.doubleByte != 2 || siteDblByte)
                cmd.append("\x1b\x4f\x43");
        }
    }
    else if (destCol < terminal->cursorColumn())
    {
        for (int i = terminal->cursorColumn(); i > destCol; i--)
        {
            if (row[i].attr.f.doubleByte != 2 || siteDblByte)
                cmd.append("\x1b\x4f\x44");
        }
    }
    if (cmd.size() > 0)
        emit q_ptr->hasBytesToSend(cmd);
}

void ViewPrivate::selectWordAround(int r, int c)
{
    BBS::Cell *cell = terminal->cellsAtRow(r);
    while (c >= 0)
    {
        if (isAlphanumeric(cell[c].byte) && !cell[c].attr.f.doubleByte)
            selectedStart = r * column + c;
        else
            break;
        c--;
    }
    c++;
    while (c < column)
    {
        if (isAlphanumeric(cell[c].byte) && !cell[c].attr.f.doubleByte)
            selectedLength++;
        else
            break;
        c++;
    }
}

int ViewPrivate::characterFromKeyPress(
        int key, Qt::KeyboardModifiers mod, bool *ok)
{
    *ok = false;
    if (!(mod & Qt::ControlModifier))
        return '\0';

    // See http://en.wikipedia.org/wiki/ASCII#ASCII_control_characters
    // for a complete list of control character sequences
    if (mod & Qt::ShiftModifier)
    {
        switch (key)
        {
        case Qt::Key_2:
            key = Qt::Key_At;
            break;
        case Qt::Key_6:
            key = Qt::Key_AsciiCircum;
            break;
        case Qt::Key_Minus:
            key = Qt::Key_Underscore;
            break;
        case Qt::Key_Slash:
            key = Qt::Key_Question;
            break;
        default:
            break;
        }
    }
    if (key < Qt::Key_At || key > Qt::Key_Underscore)
        return '\0';

    *ok = true;
    return (key - Qt::Key_At);
}

void ViewPrivate::addActionsToContextMenu(QMenu *menu)
{
    Q_Q(View);

    QString s = selection();
    QString shortUrl = shortUrlFromString(s);
    QString longUrl = longUrlFromString(s);

    // A. Treat long URL
    if (isUrlLike(longUrl))
    {
        // Split the selected text into blocks seperated by one of the
        // characters in seps
        QStringList blocks = longUrl.split(QRegExp("\\s+"));

        // Parse the blocks into two types:
        // 1. Only the first component has a protocol prefix
        // 2. Either the first one does not have a protocol prefix, or there
        //    are multiple entries in the list that has a prefix.
        bool firstOnlyProtocol = false;
        if (blocks.size() > 1 && hasProtocolPrefix(blocks.at(0)))
        {
            firstOnlyProtocol = true;
            foreach (const QString &c, blocks.mid(1))
            {
                if (hasProtocolPrefix(c))
                {
                    firstOnlyProtocol = false;
                    break;
                }
            }
        }

        // Type 1 (firstOnlyProtocol = true)
        // Concatenate all components into one single URL
        if (firstOnlyProtocol)
        {
            addUrlToMenu(blocks.join(""), menu);
        }
        // Type 2 (firstOnlyProtocol = false)
        // Treat each component as an individual URL
        else
        {
            QStringList urls;
            foreach (const QString &c, blocks)
            {
                if (isUrlLike(c))
                    urls << c;
            }
            if (!urls.isEmpty())
            {
                QString title = (urls.size() == 1) ?
                            urls.first() :
                            QObject::tr("Open as multiple URLs");
                QAction *action = menu->addAction(title, q, SLOT(openUrl()));
                action->setData(urls);
            }
        }
    }

    // B. Treat short URL
    // -> Niconico (starts with "sm" and follows with 1-8 digits)
    if (QRegExp("sm\\d{1,8}").exactMatch(shortUrl))
    {
        addUrlToMenu(QString("http://www.nicovideo.jp/watch/%1").arg(shortUrl),
                     menu);
    }
    // -> Pixiv illust (starts with "id=" and follows with 1-9 digits)
    if (QRegExp("id=\\d{1,9}").exactMatch(shortUrl))
    {
        QString url = QString("http://www.pixiv.net/member_illust.php?"
                              "mode=medium&illust_%1").arg(shortUrl);
        addUrlToMenu(url, menu);
    }
    // -> Pixiv member (starts with "mid=" and follows with 1-8 digits)
    if (QRegExp("mid=\\d{1,8}").exactMatch(shortUrl))
    {
        QString url = QString("http://www.pixiv.net/member.php?"
                              "id=_%1").arg(shortUrl.mid(4));
        addUrlToMenu(url, menu);
    }
    // -> goo.gl (4 or more alphanumeric characters)
    if (QRegExp("[A-Za-z0-9]{4,}").exactMatch(shortUrl))
    {
        QString url = QString("http://goo.gl/%1").arg(shortUrl);
        addUrlToMenu(url, menu);
    }
    // -> ppt.cc (4 characters)
    if (shortUrl.size() == 4)
    {
        addUrlToMenu(QString("http://ppt.cc/%1").arg(shortUrl), menu);
    }
    // -> 0rz.tw (5 characters)
    if (shortUrl.size() == 5)
    {
        addUrlToMenu(QString("http://0rz.tw/%1").arg(shortUrl), menu);
    }
    // -> TinyURL (7 characters)
    if (shortUrl.size() == 7)
    {
        addUrlToMenu(QString("http://tinyurl.com/%1").arg(shortUrl), menu);
    }

    // C. Other menu entries
    if (!menu->isEmpty())
        menu->addSeparator();
    if (!s.isEmpty())
    {
        QAction *action = menu->addAction("Google", q, SLOT(google()));
        action->setData(s);

        // copy() calculates the selection by itself, so we don't need to
        // provide user data here
        menu->addSeparator();
#ifdef Q_OS_MAC
        menu->addAction(QObject::tr("Copy", "Mac"), q, SLOT(copy()));
#else
        menu->addAction(QObject::tr("Copy", "General"), q, SLOT(copy()));
#endif
    }
    if (q->isConnected())
    {
        const QMimeData *mime = QApplication::clipboard()->mimeData();
        if (mime->hasText())
            menu->addAction(QObject::tr("Paste"), q, SLOT(paste()));
        if (mime->hasFormat(ANSI_COLOR_MIME))
            menu->addAction(QObject::tr("Paste Color"), q, SLOT(pasteColor()));
    }
}

void ViewPrivate::handleArrowKey(int key)
{
    QByteArray arrow("\x1b\x4f");
    switch (key)
    {
    case Qt::Key_Up:
        arrow.append('A');
        break;
    case Qt::Key_Down:
        arrow.append('B');
        break;
    case Qt::Key_Right:
        arrow.append('C');
        break;
    case Qt::Key_Left:
        arrow.append('D');
        break;
    default:
        return;
    }
    int row = terminal->cursorRow();
    int column = terminal->cursorColumn();
    terminal->updateDoubleByteStateForRow(row);
    if (terminal->connection()->site()->manualDoubleByte())
    {
        if ((key == Qt::Key_Right &&
             terminal->attributeOfCellAt(row, column).f.doubleByte == 1) ||
            (key == Qt::Key_Left &&
             terminal->attributeOfCellAt(row, column - 1).f.doubleByte == 2))
        {
            arrow.append(arrow);
        }
    }
    emit q_ptr->hasBytesToSend(arrow);
}

void ViewPrivate::handleJumpKey(int key)
{
    QByteArray bytes("\x1b[");
    switch (key)
    {
    case Qt::Key_PageUp:
        bytes.append('5');
        break;
    case Qt::Key_PageDown:
        bytes.append('6');
        break;
    case Qt::Key_Home:
        bytes.append('1');
        break;
    case Qt::Key_End:
        bytes.append('4');
        break;
    default:
        return;
    }
    bytes.append('~');
    emit q_ptr->hasBytesToSend(bytes);
}

void ViewPrivate::handleForwardDeleteKey()
{
    QByteArray bytes("\x1b[3~");
    if (terminal->connection()->site()->manualDoubleByte())
    {
        int x = terminal->cursorColumn();
        int y = terminal->cursorRow();
        if (x < column - 1 &&
            terminal->attributeOfCellAt(y, x + 1).f.doubleByte == 2)
        {
            bytes.append(bytes);
        }

    }
    emit q_ptr->hasBytesToSend(bytes);
}

void ViewPrivate::handleBackspaceKey()
{
    QByteArray bytes("\x08");
    int x = terminal->cursorColumn();
    int y = terminal->cursorRow();
    if (terminal->connection()->site()->manualDoubleByte())
    {
        if (x > 1 && terminal->attributeOfCellAt(y, x - 1).f.doubleByte == 2)
        {
            bytes.append(bytes);
        }

    }
    emit q_ptr->hasBytesToSend(bytes);
}

void ViewPrivate::handleAsciiDelete()
{
    QByteArray buffer("\x08");
    int row = terminal->cursorRow();
    int column = terminal->cursorColumn();
    if (terminal->connection()->site()->manualDoubleByte() &&
        terminal->attributeOfCellAt(row, column - 1).f.doubleByte == 2 &&
        column > 0)
    {
        buffer.append(buffer);
    }
    emit q_ptr->hasBytesToSend(buffer);
}

void ViewPrivate::drawSpecialSymbol(
        ushort code, int row, int column,
        BBS::CellAttribute left, BBS::CellAttribute right)
{
    //  0   1   2
    //
    //  3   4   5
    //
    //  6   7   8
    //
    int w = cellWidth;
    int h = cellHeight;
    int xs[9] = {column * w, (column + 1) * w, (column + 2) * w,
                 column * w, (column + 1) * w, (column + 2) * w,
                 column * w, (column + 1) * w, (column + 2) * w};
    int ys[9] = {row * h, row * h, row * h,
                 row * h + h/2, row * h + h/2, row * h + h/2,
                 (row + 1) * h, (row + 1) * h, (row + 1) * h};
    painter->begin(backImage);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    QPoint points[4];
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
        painter->fillRect(
                xs[0], ys[6] - h * (code - 0x2580) / 8, w,
                h * (code - 0x2580) / 8,
                prefs->fColor(left.f.foregroundColorCode(),
                              left.f.isForegroundBright()));
        painter->fillRect(
                xs[1], ys[7] - h * (code - 0x2580) / 8, w,
                h * (code - 0x2580) / 8,
                prefs->fColor(right.f.foregroundColorCode(),
                              right.f.isForegroundBright()));
        break;
    case 0x2589:    // ▉ Left seven eights block
    case 0x258a:    // ▊ Left three quarters block
    case 0x258b:    // ▋ Left five eighths block
        painter->fillRect(
                xs[0], ys[0], w, h,
                prefs->fColor(left.f.foregroundColorCode(),
                              left.f.isForegroundBright()));
        painter->fillRect(
                xs[1], ys[1], w * (0x258c - code) / 8, h,
                prefs->fColor(right.f.foregroundColorCode(),
                              right.f.isForegroundBright()));
        break;
    case 0x258c:    // ▌ Left half block
    case 0x258d:    // ▍ Left three eighths block
    case 0x258e:    // ▎ Left one quarter block
    case 0x258f:    // ▏ Left one eighth block
        painter->fillRect(
                xs[0], ys[0], w * (0x2590 - code) / 8, h,
                prefs->fColor(left.f.foregroundColorCode(),
                              left.f.isForegroundBright()));
        break;
    case 0x25e2:    // ◢ Black lower right triangle
        //painter->setPen(Qt::SolidLine);
        painter->setBrush(QBrush(prefs->fColor(left.f.foregroundColorCode(),
                                               left.f.isForegroundBright()),
                                 Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[7], ys[7]);
        points[2] = QPoint(xs[6], ys[6]);
        painter->drawPolygon(points, 3);
        painter->setBrush(QBrush(prefs->fColor(right.f.foregroundColorCode(),
                                               right.f.isForegroundBright()),
                                    Qt::SolidPattern));
        points[2] = QPoint(xs[8], ys[8]);
        points[3] = QPoint(xs[2], ys[2]);
        painter->drawPolygon(points, 4);
        break;
    case 0x25e3:    // ◣ Black lower left triangle
        painter->setBrush(QBrush(prefs->fColor(left.f.foregroundColorCode(),
                                               left.f.isForegroundBright()),
                                    Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[7], ys[7]);
        points[2] = QPoint(xs[6], ys[6]);
        points[3] = QPoint(xs[0], ys[0]);
        painter->drawPolygon(points, 4);
        painter->setBrush(QBrush(prefs->fColor(right.f.foregroundColorCode(),
                                               right.f.isForegroundBright()),
                                    Qt::SolidPattern));
        points[2] = QPoint(xs[8], ys[8]);
        painter->drawPolygon(points, 3);
        break;
    case 0x25e4:    // ◤ Black upper left triangle
        painter->setBrush(QBrush(prefs->fColor(left.f.foregroundColorCode(),
                                               left.f.isForegroundBright()),
                                    Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[1], ys[1]);
        points[2] = QPoint(xs[0], ys[0]);
        points[3] = QPoint(xs[6], ys[6]);
        painter->drawPolygon(points, 4);
        painter->setBrush(QBrush(prefs->fColor(right.f.foregroundColorCode(),
                                               right.f.isForegroundBright()),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[2], ys[2]);
        painter->drawPolygon(points, 3);
        break;
    case 0x25e5:    // ◥ Black upper right triangle
        painter->setBrush(QBrush(prefs->fColor(left.f.foregroundColorCode(),
                                               left.f.isForegroundBright()),
                                Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[1], ys[1]);
        points[2] = QPoint(xs[0], ys[0]);
        painter->drawPolygon(points, 3);
        painter->setBrush(QBrush(prefs->fColor(right.f.foregroundColorCode(),
                                               right.f.isForegroundBright()),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[2], ys[2]);
        points[3] = QPoint(xs[8], ys[8]);
        painter->drawPolygon(points, 4);
        break;
    case 0x25fc:    // ◼ Black medium square    // paint as a full block
        painter->fillRect(xs[0], ys[0], w, h,
                           prefs->fColor(left.f.foregroundColorCode(),
                                         left.f.isForegroundBright()));
        painter->fillRect(xs[1], ys[1], w, h,
                           prefs->fColor(right.f.foregroundColorCode(),
                                         right.f.isForegroundBright()));
        break;
    default:
        break;
    }
    painter->end();
}

void ViewPrivate::drawDoubleColor(
        ushort code, int row, int column,
        BBS::CellAttribute left, BBS::CellAttribute right)
{
    int dblPadLeft = prefs->doubleByteFontPaddingLeft();
    int dblPadBottom = prefs->doubleByteFontPaddingBottom();
    QFont dblFont = prefs->doubleByteFont();

    // Left side
    QPixmap lp(cellWidth, cellHeight);
    lp.fill(prefs->bColor(left.f.backgroundColorCode(),
                          left.f.isBackgroundBright()));
    painter->begin(&lp);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setFont(dblFont);
    painter->setPen(prefs->fColor(left.f.foregroundColorCode(),
                                  left.f.isForegroundBright()));
    painter->drawText(dblPadLeft, cellHeight - dblPadBottom, QChar(code));
    painter->end();

    // Right side
    QPixmap rp(cellWidth, cellHeight);
    rp.fill(prefs->bColor(right.f.backgroundColorCode(),
                          right.f.isBackgroundBright()));
    painter->begin(&rp);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setFont(dblFont);
    painter->setPen(prefs->fColor(right.f.foregroundColorCode(),
                                  right.f.isForegroundBright()));
    painter->drawText(dblPadLeft - cellWidth, cellHeight - dblPadBottom,
                       QChar(code));
    painter->end();

    // Draw the left half of left side, right half of the right side
    painter->begin(backImage);
    painter->setBackgroundMode(Qt::TransparentMode);
    painter->drawPixmap(column * cellWidth, row * cellHeight, lp);
    painter->drawPixmap((column + 1) * cellWidth, row * cellHeight, rp);
    painter->end();
}

void ViewPrivate::paintSelection()
{
    int loc;
    int len;
    if (selectedLength >= 0)
    {
        loc = selectedStart;
        len = selectedLength;
    }
    else
    {
        loc = selectedStart + selectedLength;
        len = 0 - selectedLength;
    }
    int x = loc % column;
    int y = loc / column;

    Qt::BGMode bgm = painter->backgroundMode();
    QPen p = painter->pen();
    painter->setPen(Qt::NoPen);
    painter->setBackgroundMode(Qt::TransparentMode);
    painter->setBrush(QBrush(QColor(153, 229, 153, 102), Qt::SolidPattern));
    while (len > 0)
    {
        if (x + len < column)
        {
            painter->drawRect(x * cellWidth, y * cellHeight,
                               len * cellWidth, cellHeight);
            len = 0;
        }
        else
        {
            painter->drawRect(x * cellWidth, y * cellHeight,
                               (column - x) * cellWidth, cellHeight);
            len -= column - x;
        }
        x = 0;
        y++;
    }
    painter->setPen(p);
    painter->setBackgroundMode(bgm);
}

void ViewPrivate::paintBlink(QRect &r)
{
    if (!q_ptr->isConnected() || !blinkTicker)
        return;

    for (int y = r.top() / cellHeight; y < r.bottom() / cellHeight + 1; y++)
    {
        if (y < 0 || y >= row)
            continue;
        BBS::Cell *cells = terminal->cellsAtRow(y);
        for (int x = r.left() / cellWidth; x < r.right() / cellWidth + 1; x++)
        {
            if (x < 0 || x >= column)
                continue;
            const BBS::CellAttribute &a = cells[x].attr;
            if (!a.f.blinking)
                continue;
            painter->setPen(Qt::NoPen);
            painter->setBrush(QBrush(prefs->bColor(a.f.backgroundColorCode(),
                                                   a.f.isBackgroundBright())));
            painter->drawRect(x * cellWidth, y * cellHeight,
                              cellWidth, cellHeight);
        }
    }
}

void ViewPrivate::refreshHiddenRegion()
{

}

void ViewPrivate::clearSelection()
{
    Q_Q(View);

    if (selectedLength)
    {
        int start = selectedLength > 0 ?
                    selectedStart : selectedStart + selectedLength - 1;
        int length = selectedLength > 0 ?
                    selectedLength : 0 - (selectedLength - 1);
        int startY = start / column;
        int endY = (start + length) / column;
        if (startY == endY)
        {
            q->update(start % column * cellWidth, startY * cellHeight,
                      length * cellWidth, cellHeight);
        }
        else
        {
            q->update(0, startY * cellHeight, column * cellWidth,
                      (endY - startY + 1) * cellHeight);
        }
        selectedLength = 0;
    }
}

void ViewPrivate::updateText(int row, int column)
{
    int sglPadLeft = prefs->defaultFontPaddingLeft();
    int sglPadBott = prefs->defaultFontPaddingBottom();
    int dblPadLeft = prefs->doubleByteFontPaddingLeft();
    int dblPadBott = prefs->doubleByteFontPaddingBottom();
    QFont sglFont = prefs->defaultFont();
    QFont dblFont = prefs->doubleByteFont();
    BBS::Cell *cells = terminal->cellsAtRow(row);
    BBS::CellAttribute &attr = cells[column].attr;
    ushort code;
    switch (attr.f.doubleByte)
    {
    case 0: // Not double byte
        painter->begin(backImage);
        painter->setRenderHint(QPainter::TextAntialiasing);
        code = cells[column].byte ? cells[column].byte : ' ';
        if (attr.f.isHidden() && prefs->showHiddenText())
        {
            // Draw outline font for hidden text
            QPainterPath path;
            path.addText(column * cellWidth + sglPadLeft,
                         (row + 1) * cellHeight - sglPadBott,
                         sglFont, QChar(code));
            painter->setPen(QColor(Qt::white));
            painter->drawPath(path);
        }
        else
        {
            // Draw normally
            painter->setPen(prefs->fColor(attr.f.foregroundColorCode(),
                                          attr.f.isForegroundBright()));
            painter->setFont(sglFont);
            painter->drawText(column * cellWidth + sglPadLeft,
                              (row + 1) * cellHeight - sglPadBott,
                              QChar(code));
        }
        painter->end();
        break;
    case 1: // First half of double byte
        break;
    case 2:
        switch (terminal->connection()->site()->encoding())
        {
        case BBS::EncodingBig5:
            code = YL::B2U[(static_cast<ushort>(cells[column - 1].byte) << 8) +
                           (static_cast<ushort>(cells[column].byte) - 0x8000)];
            break;
        case BBS::EncodingGBK:
            code = YL::G2U[(static_cast<ushort>(cells[column - 1].byte) << 8) +
                           (static_cast<ushort>(cells[column].byte) - 0x8000)];
            break;
        default:    // Don't convert
            code = ((static_cast<ushort>(cells[column - 1].byte) << 8) +
                    (static_cast<ushort>(cells[column].byte) - 0x8000));
            break;
        }
        if (isSpecialSymbol(code))
        {
            drawSpecialSymbol(code, row, column - 1,
                                 cells[column - 1].attr, cells[column].attr);
        }
        else
        {
            if (cells[column - 1].attr.f.foregroundColorCode()
                    != cells[column].attr.f.foregroundColorCode() ||
                cells[column - 1].attr.f.isForegroundBright()
                    != cells[column].attr.f.isForegroundBright())
            {
                drawDoubleColor(code, row, column - 1,
                                 cells[column - 1].attr, cells[column].attr);
            }
            else
            {
                painter->begin(backImage);
                painter->setRenderHint(QPainter::TextAntialiasing);
                painter->setPen(prefs->fColor(attr.f.foregroundColorCode(),
                                              attr.f.isForegroundBright()));
                if (attr.f.isHidden() && prefs->showHiddenText())
                {
                    // Draw outline font for hidden text
                    QPainterPath path;
                    path.addText((column - 1) * cellWidth + dblPadLeft,
                                 (row + 1) * cellHeight - dblPadBott,
                                 dblFont, QChar(code));
                    painter->setPen(QColor(Qt::white));
                    painter->drawPath(path);
                }
                else
                {
                    // Draw normally
                    painter->setFont(dblFont);
                    painter->drawText((column - 1) * cellWidth + dblPadLeft,
                                         (row + 1) * cellHeight - dblPadBott,
                                         QChar(code));
                }
                painter->end();
            }
        }
    }
}

void ViewPrivate::displayCellAt(int column, int row)
{
    q_ptr->update(column * cellWidth, row * cellHeight, cellWidth, cellHeight);
}

QString ViewPrivate::selection() const
{
    if (!selectedLength || selectedStart == PositionNotFound)
        return "";
    int start = selectedStart;
    int length = selectedLength;
    if (length < 0)
    {
        start += length;
        length = 0 - length;
    }
    return terminal->stringFromIndex(start, length);
}

void ViewPrivate::updateCursor(
        const QPoint &pos, Qt::KeyboardModifiers modifiers,
        Qt::MouseButtons buttons)
{
    Q_Q(View);
    if (modifiers & UJ::ModModifier)
        q->setCursor(Qt::IBeamCursor);
    else if (terminal->hasUrlAt(indexFromPoint(pos)) && !buttons)
        q->setCursor(Qt::PointingHandCursor);
    else
        q->unsetCursor();
}

void ViewPrivate::addUrlToMenu(const QString &url, QMenu *menu) const
{
    QAction *action = menu->addAction(url, q_ptr, SLOT(openUrl()));
    action->setData(url);
}

void ViewPrivate::handleKeyPress(QKeyEvent *e)
{
    Q_Q(View);
        int key = e->key();
        if (key != UJ::Key_Mod)
            clearSelection();
        terminal->setHasMessage(false);

        bool ok = false;
        Qt::KeyboardModifiers modifiers = e->modifiers();
        const QString &text = e->text();

        updateCursor(q->mapFromGlobal(QCursor::pos()), modifiers, 0);

        if (modifiers & Qt::ControlModifier)
        {
            int c = characterFromKeyPress(key, modifiers, &ok);
            if (ok)
                emit q->hasBytesToSend(QByteArray(1, c));
        }
        else    // No modifier
        {
            switch (key)
            {
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Right:
            case Qt::Key_Left:
                handleArrowKey(key);
                break;
            case Qt::Key_PageUp:
            case Qt::Key_PageDown:
            case Qt::Key_Home:
            case Qt::Key_End:
                handleJumpKey(key);
                break;
            case Qt::Key_Delete:
                handleForwardDeleteKey();
                break;
            case Qt::Key_Backspace:
                handleBackspaceKey();
                break;
            case 0x7f:
                handleAsciiDelete();
                break;
            default:
                emit q->hasBytesToSend(text.toLatin1());   // Normal text input
                break;
            }
        }
        e->accept();
}

void ViewPrivate::handleKeyRelease(QKeyEvent *e)
{
    Q_Q(View);
    if (q->isConnected())
        updateCursor(q->mapFromGlobal(QCursor::pos()), e->modifiers(), 0);
}

void ViewPrivate::handleInputMethod(QInputMethodEvent *)
{
    // Put the holder near the cursor. Usually we put it one row above
    // it (with some extra cusion, so it's 1.2 instead of 1), but if the
    // cursor is at row 0 or 1, put it one row UNDER it instead.
    QPoint p = pointFromIndex(terminal->cursorColumn(), terminal->cursorRow());
    if (p.y() <= cellHeight * 2)
        p.ry() += cellHeight * 1.2;
    else
        p.ry() -= cellHeight * 1.2;
    preeditHolder->move(p);
}

}   // namespace Qelly

}   // namespace UJ
