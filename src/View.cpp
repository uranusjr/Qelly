/*****************************************************************************
 * View.cpp
 *
 * Created: 06/10 2011 by uranusjr
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

#include "View.h"
#include <QByteArray>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPointF>
#include <QTextCodec>
#include <QTimer>
#include <QUrl>
#include "AbstractConnection.h"
#include "Encodings.h"
#include "Globals.h"
#include "Terminal.h"
#include "SharedPreferences.h"
#include "Site.h"
#include "UJCommonDefs.h"
#include <QDebug>

namespace UJ
{

namespace Qelly
{

View::View(QWidget *parent) : Widget(parent)
{
    _backImage = 0;
    _terminal = 0;
    _prefs = SharedPreferences::sharedInstance();
    buildInfo();
    setFocusPolicy(Qt::ClickFocus);
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_KeyCompression, false); // One key per key event
    setFixedSize(_cellWidth * _column, _cellHeight * _row);
}

View::~View()
{
    delete _backImage;
}

void View::buildInfo()
{
    _cellWidth = _prefs->cellWidth();
    _cellHeight = _prefs->cellHeight();
    _row = BBS::SizeRowCount;
    _column = BBS::SizeColumnCount;

    if (_backImage)
        delete _backImage;
    _backImage = new QPixmap(_cellWidth * _column, _cellHeight * _row);
    _backImageFlipped = false;
    if (_singleAdvances.isEmpty() || _doubleAdvances.isEmpty())
    {
        _singleAdvances.clear();
        _doubleAdvances.clear();
        for (int i = 0; i < _column; i++)
        {
            _singleAdvances << QSize(_cellWidth * 1, 0);
            _doubleAdvances << QSize(_cellWidth * 2, 0);
        }
    }

    _insertBuffer.clear();
    _insertTimer = new QTimer(this);
    connect(_insertTimer, SIGNAL(timeout()), this, SLOT(popInsertBuffer()));

    _selectedStart = PositionNotFound;
    _selectedLength = 0;
    _markedStart = PositionNotFound;
    _markedLength = 0;
    // NOTE: Set _textField hidden...This is the MarkedTextView thingy
}

int View::indexFromPoint(QPoint p)
{
    if (p.rx() >= _column * _cellWidth)
        p.setX(_column * _cellWidth - 1);
    else if (p.rx() < 0)
        p.setX(0);
    if (p.ry() >= _row * _cellHeight)
        p.setY(_row * _cellHeight - 1);
    else if (p.ry() < 0)
        p.setY(0);
    int x = p.rx() / _cellWidth;
    int y = p.ry() / _cellHeight;

    return y * _column + x;
}

void View::mousePressEvent(QMouseEvent *e)
{
    setFocus(Qt::MouseFocusReason);
    if (isConnected())
    {
        terminal()->setHasMessage(false);
        _selectedStart = indexFromPoint(e->pos());
        _selectedLength = 0;
        update();

        // META + click = move the cursor
        if (e->modifiers() & UJ::MOD)
            moveCursorTo(_selectedStart / _column, _selectedStart % _column);
    }

    return Qx::Widget::mousePressEvent(e);
}

void View::moveCursorTo(int destRow, int destCol)
{
    QByteArray cmd;
    bool needsVertical = false;
    if (destRow > terminal()->cursorRow())
    {
        needsVertical = true;
        cmd.append('\x01');
        for (int i = terminal()->cursorRow(); i < destRow; i++)
            cmd.append("\x1b\x4f\x42");
    }
    else if (destRow < terminal()->cursorRow())
    {
        needsVertical = true;
        cmd.append('\x01');
        for (int i = terminal()->cursorRow(); i < destRow; i++)
            cmd.append("\x1b\x4f\x41");
    }

    BBS::Cell *row = terminal()->cellsAtRow(destRow);
    bool siteDblByte = terminal()->connection()->site()->manualDoubleByte();
    if (needsVertical)
    {
        for (int i = 0; i < destCol; i++)
        {
            if (row[i].attr.f.doubleByte != 2 || siteDblByte)
                cmd.append("\x1b\x4f\x43");
        }
    }
    else if (destCol > terminal()->cursorColumn())
    {
        for (int i = terminal()->cursorColumn(); i < destCol; i++)
        {
            if (row[i].attr.f.doubleByte != 2 || siteDblByte)
                cmd.append("\x1b\x4f\x43");
        }
    }
    else if (destCol < terminal()->cursorColumn())
    {
        for (int i = terminal()->cursorColumn(); i < destCol; i++)
        {
            if (row[i].attr.f.doubleByte != 2 || siteDblByte)
                cmd.append("\x1b\x4f\x44");
        }
    }
    if (cmd.size() > 0)
        emit hasBytesToSend(cmd);
}

void View::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (isConnected() && !(e->modifiers() & UJ::MOD))
    {
        int r = _selectedStart / _column;
        int c = _selectedStart % _column;
        terminal()->updateDoubleByteStateForRow(r);
        BBS::Cell *row = terminal()->cellsAtRow(r);
        switch (row[c].attr.f.doubleByte == 2)
        {
        case 1: // First half of double byte
            _selectedLength = 2;
            break;
        case 2: // Second half of double byte
            _selectedStart -= 1;
            _selectedLength = 2;
            break;
        default:
            // Try to select the whole word/domain name on double click
            if (isAlphanumeric(row[c].byte))
                selectWordAround(r, c);
            else
                _selectedLength = 1;
        }
    }

    return Qx::Widget::mouseDoubleClickEvent(e);
}

void View::selectWordAround(int row, int column)
{
    BBS::Cell *c = terminal()->cellsAtRow(row);
    while (column >= 0)
    {
        if (isAlphanumeric(c[column].byte) && !c[column].attr.f.doubleByte)
            _selectedStart = row * _column + column;
        else
            break;
        column--;
    }
    column++;
    while (column < _column)
    {
        if (isAlphanumeric(c[column].byte) && !c[column].attr.f.doubleByte)
            _selectedLength++;
        else
            break;
        column++;
    }
}

void View::mouseTripleClickEvent(QMouseEvent *e)
{
    if (isConnected() && !(e->modifiers() & UJ::MOD))
    {
        // Select the whole line on triple click
        _selectedStart = _selectedStart - (_selectedStart % _column);
        _selectedLength = _column;
    }

    return Qx::Widget::mouseTripleClickEvent(e);
}

void View::mouseMoveEvent(QMouseEvent *e)
{
    if (isConnected())
    {
        int index = indexFromPoint(e->pos());
        int old = _selectedLength;
        _selectedLength = index - _selectedStart + 1;
        if (_selectedLength <= 0)
            _selectedLength -= 1;
        if (old != _selectedLength)
            update();   // NOTE: Update which region?
    }
    return Qx::Widget::mouseMoveEvent(e);
}

void View::mouseReleaseEvent(QMouseEvent *e)
{
    if (!_selectedLength && isConnected())
    {
        int index = indexFromPoint(e->pos());
        bool hasUrl = false;
        QString url = terminal()->urlStringAt(index / _column, index % _column,
                                              &hasUrl);
        if (hasUrl && !(e->modifiers() & UJ::MOD))
        {
            // NOTE: Should we implement image previewer at all?
            QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
        }
    }

    return Qx::Widget::mouseReleaseEvent(e);
}

void View::keyPressEvent(QKeyEvent *e)
{
    if (isConnected())
    {
        clearSelection();
        terminal()->setHasMessage(false);
        QString text = e->text();
        if (text.isEmpty())   // Special key (up, down, etc.) or modified
        {
            int key = e->key();
            Qt::KeyboardModifiers modifiers = e->modifiers();
            bool ok = false;
            switch (key)
            {
            case Qt::Key_Tab:
                emit hasBytesToSend(QByteArray("\x09"));
                break;
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
            case 0x7f:
                //handleAsciiDelete();
                break;
            default:
                if (modifiers & Qt::ControlModifier)
                {
                    char c = characterFromKeyPress(key, modifiers, &ok);
                    if (ok)
                        emit hasBytesToSend(QByteArray(1, c));
                }
            }
        }
        else    // Normal input
        {
            QByteArray bytes;
            bytes.append(text);
            emit hasBytesToSend(bytes);
        }
    }

    return Qx::Widget::keyPressEvent(e);
}

void View::handleForwardDeleteKey()
{
    QByteArray bytes("\x1b[3~");
    if (terminal()->connection()->site()->manualDoubleByte())
    {
        int x = terminal()->cursorColumn();
        int y = terminal()->cursorRow();
        if (x < _column - 1 &&
            terminal()->attributeOfCellAt(y, x + 1).f.doubleByte == 2)
        {
            bytes.append(bytes);
        }

    }
    emit hasBytesToSend(bytes);
}

void View::handleArrowKey(int key)
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
    int row = terminal()->cursorRow();
    int column = terminal()->cursorColumn();
    terminal()->updateDoubleByteStateForRow(row);
    if (terminal()->connection()->site()->manualDoubleByte())
    {
        if ((key == Qt::Key_Right &&
             terminal()->attributeOfCellAt(row, column).f.doubleByte == 1) ||
            (key == Qt::Key_Left &&
             terminal()->attributeOfCellAt(row, column - 1).f.doubleByte == 2))
        {
            arrow.append(arrow);
        }
    }
    emit hasBytesToSend(arrow);
}

void View::handleJumpKey(int key)
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
    emit hasBytesToSend(bytes);
}

void View::handleAsciiDelete()
{
    QByteArray buffer("\x08");
    int row = terminal()->cursorRow();
    int column = terminal()->cursorColumn();
    if (terminal()->connection()->site()->manualDoubleByte() &&
        terminal()->attributeOfCellAt(row, column - 1).f.doubleByte == 2 &&
        column > 0)
    {
        buffer.append(buffer);
    }
    emit hasBytesToSend(buffer);
}

int View::characterFromKeyPress(int key, Qt::KeyboardModifiers mod, bool *ok)
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

void View::inputMethodEvent(QInputMethodEvent *e)
{
    if (isConnected())
    {
        QString commit = e->commitString();
        if (commit.isEmpty())
        {
            if (e->preeditString() != _preeditString)
            {
                _preeditString = e->preeditString();
                // NOTE: draw the preedit string on screen
            }
        }
        else
        {
            _preeditString = QString();
            // Un-draw the preedit string on screen
            insertText(commit);
        }
    }

    Qx::Widget::inputMethodEvent(e);
}

void View::insertText(QString &string, uint delayMs)
{

    QByteArray bytes;
    QString &s = string.replace('\n', '\r');
    QChar lastAscii = QChar('\x7f');
    for (int i = 0; i < s.size(); i++)
    {
        QChar c = s[i];
        if (c < lastAscii)
        {
            uchar b = c.unicode() % 0x100;
            if (!delayMs)
                bytes.append(b);
            else
                _insertBuffer.enqueue(b);
        }
        else
        {
            ushort code;
            switch (terminal()->connection()->site()->encoding())
            {
            case BBS::EncodingBig5:
                code = YL::U2B[c.unicode()];
                break;
            case BBS::EncodingGBK:
                code = YL::U2G[c.unicode()];
                break;
            default:
                break;
            }
            uchar bu = c.unicode() / 0x100;
            uchar bl = c.unicode() % 0x100;
            if (!delayMs)
            {
                bytes.append(bu);
                bytes.append(bl);
            }
            else
            {
                _insertBuffer.enqueue(bu);
                _insertBuffer.enqueue(bl);
            }
        }
    }

    if (!delayMs)
        emit hasBytesToSend(bytes);
    else
        _insertTimer->start(delayMs);
}

void View::popInsertBuffer()
{
    emit hasBytesToSend(QByteArray(1, _insertBuffer.dequeue()));
    if (_insertBuffer.isEmpty())
        _insertTimer->stop();
}

void View::clearSelection()
{
    if (_selectedLength)
    {
        _selectedLength = 0;
        update();
    }
}

void View::updateScreen()
{
    if (!isConnected())
        return;

    updateBackImage();
    int x = terminal()->cursorColumn();
    int y = terminal()->cursorRow();
    if (_x != x || _y != y)
    {
        //displayCellAt(_x, _y);  // Un-draw the old cursor
        _x = x;
        _y = y;
    }
    //displayCellAt(_x, _y);    // Draw current cursor
    update();
}

void View::updateBackImage()
{
    for (int y = 0; y < _row; y++)
    {
        // Background
        for (int x = 0; x < _column; x++)
        {
            if (terminal()->isDiryAt(y, x))
            {
                int start = x;
                while (x < _column && terminal()->isDiryAt(y, x))
                    x++;
                updateBackground(y, start, x);
            }
        }

        // Foreground
        updateText(y);

        for (int x = 0; x < _column; x++)
            terminal()->setDirtyAt(y, x, false);
    }
}

void View::updateBackground(int row, int startColumn, int endColumn)
{
    BBS::Cell *cells = terminal()->cellsAtRow(row);
    BBS::CellAttribute now;
    BBS::CellAttribute last = cells[startColumn].attr;
    int length = 1;

    painter.begin(_backImage);
    for (int x = startColumn + 1; x <= endColumn; x++)
    {
        now = cells[x].attr;
        bool changed = now.f.bColorIndex != last.f.bColorIndex ||
                       (now.f.reversed & now.f.bright)
                            != (last.f.reversed & last.f.bright) ||
                       x == endColumn;
        if (changed)
        {
            painter.fillRect((x - length) * _cellWidth, row * _cellHeight,
                             length * _cellWidth, _cellHeight,
                             _prefs->bColor(last.f.bColorIndex,
                                           last.f.reversed && last.f.bright));
            length = 1;
            last = now;
        }
        else
        {
            length++;
        }
    }

    painter.end();

    QRect rect = QRect(startColumn * _cellWidth, row * _cellHeight,
                       (endColumn - startColumn) * _cellWidth, _cellHeight);
    update(rect);
}

void View::updateText(int row)
{
    terminal()->updateDoubleByteStateForRow(row);

    // Find first dirty position
    int x = 0;
    while (x < _column && !terminal()->isDiryAt(row, x))
        x++;
    if (x == _column)
        return;

    int start = x;

    for (x = start; x < _column; x++)
    {
        if (!terminal()->isDiryAt(row, x))
            continue;
        updateText(row, x);
    }
    //update((start - 1) *_cellWidth, row * _cellHeight,
    //       _cellWidth, _cellHeight);
    update();
}

void View::updateText(int row, int x)
{
    int sglPadLeft = _prefs->defaultFontPaddingLeft();
    int sglPadBott = _prefs->defaultFontPaddingBottom();
    int dblPadLeft = _prefs->doubleByteFontPaddingLeft();
    int dblPadBott = _prefs->doubleByteFontPaddingBottom();
    QFont sglFont = _prefs->defaultFont();
    QFont dblFont = _prefs->doubleByteFont();
    BBS::Cell *cells = terminal()->cellsAtRow(row);
    BBS::CellAttribute &attr = cells[x].attr;
    ushort code;
    switch (attr.f.doubleByte)
    {
    case 0: // Not double byte
        painter.begin(_backImage);
        painter.setFont(sglFont);
        painter.setPen(_prefs->fColor(attr.f.fColorIndex, attr.f.bright));
        code = cells[x].byte ? cells[x].byte : ' ';
        painter.drawText(x * _cellWidth + sglPadLeft,
                         (row + 1) * _cellHeight - sglPadBott,
                         QChar(code));
        painter.end();
        break;
    case 1: // First half of double byte
        break;
    case 2:
        switch (terminal()->connection()->site()->encoding())
        {
        case BBS::EncodingBig5:
            code = YL::B2U[(static_cast<ushort>(cells[x - 1].byte) << 8) +
                           (static_cast<ushort>(cells[x].byte) - 0x8000)];
            break;
        case BBS::EncodingGBK:
            code = YL::G2U[(static_cast<ushort>(cells[x - 1].byte) << 8) +
                           (static_cast<ushort>(cells[x].byte) - 0x8000)];
            break;
        default:    // Don't convert
            code = ((static_cast<ushort>(cells[x - 1].byte) << 8) +
                    (static_cast<ushort>(cells[x].byte) - 0x8000));
            break;
        }
        if (isSpecialSymbol(code))
        {
            drawSpecialSymbol(code, row, x - 1,
                               cells[x - 1].attr, cells[x].attr);
        }
        else
        {
            if (fColorIndex(cells[x - 1].attr) != fColorIndex(cells[x].attr) ||
                    fBright(cells[x - 1].attr) != fBright(cells[x].attr))
            {
                drawDoubleColor(code, row, x - 1,
                                 cells[x - 1].attr, cells[x].attr);
            }
            else
            {
                painter.begin(_backImage);
                painter.setFont(dblFont);
                painter.setPen(_prefs->fColor(attr.f.fColorIndex,
                                              attr.f.bright));
                painter.drawText((x - 1) * _cellWidth + dblPadLeft,
                                 (row + 1) * _cellHeight - dblPadBott,
                                 QChar(code));
                painter.end();
            }
        }
    }
}

void View::drawSpecialSymbol(ushort code, int row, int column,
                              BBS::CellAttribute left, BBS::CellAttribute right)
{
    //  0   1   2
    //
    //  3   4   5
    //
    //  6   7   8
    //
    int w = _cellWidth;
    int h = _cellHeight;
    int xs[9] = {(column - 1) * w, column * w, (column + 1) * w,
                 (column - 1) * w, column * w, (column + 1) * w,
                 (column - 1) * w, column * w, (column + 1) * w};
    int ys[9] = {row * h, row * h, row * h,
                 row * h + h/2, row * h + h/2, row * h + h/2,
                 (row + 1) * h, (row + 1) * h, (row + 1) * h};
    painter.begin(_backImage);
    painter.setPen(Qt::NoPen);
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
        painter.fillRect(xs[0], ys[6] - h * (code - 0x2580) / 8,
                         w, h * (code - 0x2580) / 8,
                         _prefs->fColor(left.f.fColorIndex, left.f.bright));
        painter.fillRect(xs[1], ys[7] - h * (code - 0x2580) / 8,
                         w, h * (code - 0x2580) / 8,
                         _prefs->fColor(right.f.fColorIndex, right.f.bright));
        break;
    case 0x2589:    // ▉ Left seven eights block
    case 0x258a:    // ▊ Left three quarters block
    case 0x258b:    // ▋ Left five eighths block
        painter.fillRect(xs[0], ys[0], w, h,
                         _prefs->fColor(left.f.fColorIndex, left.f.bright));
        painter.fillRect(xs[1], ys[1], w * (0x258c - code) / 8, h,
                         _prefs->fColor(right.f.fColorIndex, right.f.bright));
        break;
    case 0x258c:    // ▌ Left half block
    case 0x258d:    // ▍ Left three eighths block
    case 0x258e:    // ▎ Left one quarter block
    case 0x258f:    // ▏ Left one eighth block
        painter.fillRect(xs[0], ys[0], w * (0x2590 - code) / 8, h,
                         _prefs->fColor(left.f.fColorIndex, left.f.bright));
        break;
    case 0x25e2:    // ◢ Black lower right triangle
        //painter.setPen(Qt::SolidLine);
        painter.setBrush(QBrush(_prefs->fColor(left.f.fColorIndex,
                                               left.f.bright),
                                Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[7], ys[7]);
        points[2] = QPoint(xs[6], ys[6]);
        painter.drawPolygon(points, 3);
        painter.setBrush(QBrush(_prefs->fColor(right.f.fColorIndex,
                                               right.f.bright),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[8], ys[8]);
        points[3] = QPoint(xs[2], ys[2]);
        painter.drawPolygon(points, 4);
        break;
    case 0x25e3:    // ◣ Black lower left triangle
        painter.setBrush(QBrush(_prefs->fColor(left.f.fColorIndex,
                                               left.f.bright),
                                Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[7], ys[7]);
        points[2] = QPoint(xs[6], ys[6]);
        points[3] = QPoint(xs[0], ys[0]);
        painter.drawPolygon(points, 4);
        painter.setBrush(QBrush(_prefs->fColor(right.f.fColorIndex,
                                               right.f.bright),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[8], ys[8]);
        painter.drawPolygon(points, 3);
        break;
    case 0x25e4:    // ◤ Black upper left triangle
        painter.setBrush(QBrush(_prefs->fColor(left.f.fColorIndex,
                                               left.f.bright),
                                Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[1], ys[1]);
        points[2] = QPoint(xs[0], ys[0]);
        points[3] = QPoint(xs[6], ys[6]);
        painter.drawPolygon(points, 4);
        painter.setBrush(QBrush(_prefs->fColor(right.f.fColorIndex,
                                               right.f.bright),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[2], ys[2]);
        painter.drawPolygon(points, 3);
        break;
    case 0x25e5:    // ◥ Black upper right triangle
        painter.setBrush(QBrush(_prefs->fColor(left.f.fColorIndex,
                                               left.f.bright),
                                Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[1], ys[1]);
        points[2] = QPoint(xs[0], ys[0]);
        painter.drawPolygon(points, 3);
        painter.setBrush(QBrush(_prefs->fColor(right.f.fColorIndex,
                                               right.f.bright),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[2], ys[2]);
        points[3] = QPoint(xs[8], ys[8]);
        painter.drawPolygon(points, 4);
        break;
    case 0x25fc:    // ◼ Black medium square    // paint as a full block
        painter.fillRect(xs[0], ys[0], w, h,
                         _prefs->fColor(left.f.fColorIndex, left.f.bright));
        painter.fillRect(xs[1], ys[1], w, h,
                         _prefs->fColor(right.f.fColorIndex, right.f.bright));
        break;
    default:
        break;
    }
    painter.end();
}

void View::drawDoubleColor(ushort code, int row, int column,
                            BBS::CellAttribute left, BBS::CellAttribute right)
{
    int dblPadLeft = _prefs->doubleByteFontPaddingLeft();
    int dblPadBottom = _prefs->doubleByteFontPaddingBottom();
    QFont dblFont = _prefs->doubleByteFont();

    // Left side
    QPixmap lp(_cellWidth, _cellHeight);
    lp.fill(_prefs->bColor(left.f.bColorIndex));
    painter.begin(&lp);
    painter.setFont(dblFont);
    int height = painter.fontMetrics().height();
    painter.setPen(_prefs->fColor(left.f.fColorIndex, left.f.bright));
    painter.drawText(dblPadLeft, height - dblPadBottom, QChar(code));
    painter.end();

    // Right side
    QPixmap rp(_cellWidth, _cellHeight);
    rp.fill(_prefs->bColor(right.f.bColorIndex));
    painter.begin(&rp);
    painter.setFont(dblFont);
    painter.setPen(_prefs->fColor(right.f.fColorIndex, right.f.bright));
    painter.drawText(dblPadLeft - _cellWidth, height - dblPadBottom, QChar(code));
    painter.end();

    // Draw the left half of left side, right half of the right side
    painter.begin(_backImage);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.drawPixmap((column - 1) * _cellWidth, row * _cellHeight, lp);
    painter.drawPixmap(column * _cellWidth, row * _cellHeight, rp);
    painter.end();
}

void View::paintEvent(QPaintEvent *e)
{
    painter.begin(this);
    if (isConnected())
    {
        QRect r = e->rect();

        // Draw a portion of back image
        painter.drawPixmap(r.left(), r.top(), r.width(), r.height(), *_backImage);
        paintBlink(r);

        // URL line
        // NOTE: Preference for URL color and width
        painter.setPen(QPen(QColor("orange"), 1.0));
        for (int y = r.top() / _cellHeight; y <= r.bottom() / _cellHeight; y++)
        {
            BBS::Cell *cells = terminal()->cellsAtRow(y);
            int xEnd = r.right() / _cellWidth + 1;
            for (int x = r.left() / _cellWidth; x < xEnd; x++)
            {
                int start = x;
                while (start < xEnd && cells[x].attr.f.isUrl)
                    x++;
                if (start != x)
                {
                    // NOTE: Prefernce for URL y offset (the -0.5)
                    int yPos = (y + 1) * _cellHeight - 0.5;
                    painter.drawLine(start *_cellWidth, yPos,
                                     x * _cellWidth, yPos);
                }
            }
        }

        // Cursor
        // NOTE: Preference for cursor color and shape (?)
        //       Should a non-line type cursor be implemented?
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
        _x = terminal()->cursorColumn();
        _y = terminal()->cursorRow();
        // NOTE: Prefernce for cursor y offset (the -2)
        int yPos = (_y + 1) * _cellHeight - 2;
        painter.drawRect(_x * _cellWidth, yPos, _cellWidth, 2);

        // Selection
        if (_selectedLength)
            paintSelection();
    }
    else
    {
        // Clear everything in the widget
        painter.fillRect(0, 0, width(), height(), _prefs->backgroundColor());
    }

    painter.end();
    return Qx::Widget::paintEvent(e);
}

void View::paintBlink(QRect &r)
{
    if (!isConnected())
        return;

    for (int y = r.top() / _cellHeight; y <= r.bottom() / _cellHeight; y++)
    {
        BBS::Cell *cells = terminal()->cellsAtRow(y);
        for (int x = r.left() / _cellWidth; x < r.right() / _cellWidth + 1; x++)
        {
            BBS::CellAttribute a = cells[x].attr;
            if (!a.f.blinking)
                continue;
            int colorIndex = a.f.reversed ? a.f.fColorIndex : a.f.bColorIndex;
            bool bright = a.f.reversed ? a.f.bright : false;
            painter.setPen(_prefs->fColor(colorIndex, bright));
            painter.fillRect(x * _cellWidth, (y - 1) * _cellHeight,
                             _cellWidth, _cellHeight, Qt::SolidPattern);
        }
    }
}

void View::paintSelection()
{
    int loc;
    int len;
    if (_selectedLength >= 0)
    {
        loc = _selectedStart;
        len = _selectedLength;
    }
    else
    {
        loc = _selectedStart + _selectedLength;
        len = -_selectedLength;
    }
    int x = loc % _column;
    int y = loc / _column;

    Qt::BGMode bgm = painter.backgroundMode();
    QPen p = painter.pen();
    painter.setPen(Qt::NoPen);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setBrush(QBrush(QColor(153, 229, 153, 102), Qt::SolidPattern));
    while (len > 0)
    {
        if (x + len < _column)
        {
            painter.drawRect(x * _cellWidth, y * _cellHeight,
                             len * _cellWidth, _cellHeight);
            len = 0;
        }
        else
        {
            painter.drawRect(x * _cellWidth, y * _cellHeight,
                             (_column - x) * _cellWidth, _cellHeight);
            len -= _column - x;
        }
        x = 0;
        y++;
    }
    painter.setPen(p);
    painter.setBackgroundMode(bgm);
}

void View::refreshHiddenRegion()
{
}

void View::extendBottom(int start, int end)
{
    int width = _column * _cellWidth;
    int height = (end - start + 1) * _cellHeight;
    QPixmap m(width, height);
    painter.begin(&m);
    painter.drawPixmap(0, -(start * _cellHeight), width, height, *_backImage);
    painter.end();
    painter.begin(_backImage);
    painter.drawPixmap(0, (start - 1) * _cellHeight, width, height, m);
    painter.fillRect(0, end * _cellHeight, width, _cellHeight,
                     _prefs->backgroundColor());
    painter.end();
}

void View::extendTop(int start, int end)
{
    int width = _column * _cellWidth;
    int height = (end - start + 1) * _cellHeight;
    QPixmap m(width, height);
    painter.begin(&m);
    painter.drawPixmap(0, -(start * _cellHeight), width, height, *_backImage);
    painter.end();
    painter.begin(_backImage);
    painter.drawPixmap(0, (start + 1) * _cellHeight, width, height, m);
    painter.fillRect(0, start * _cellHeight, width, _cellHeight,
                     _prefs->backgroundColor());
    painter.end();
}

bool View::isConnected()
{
    return (terminal() != 0 && terminal()->connection() != 0 &&
            terminal()->connection()->isConnected());
}

void View::setTerminal(Connection::Terminal *terminal)
{
    if (_terminal == terminal)
        return;
    disconnect(_terminal);
    delete _terminal;
    _terminal = terminal;
    if (!_terminal)
        return;
    _terminal->setView(this);
    connect(_terminal, SIGNAL(dataProcessed()), this, SLOT(updateScreen()));
    connect(this, SIGNAL(hasBytesToSend(QByteArray)),
            _terminal->connection(), SLOT(sendBytes(QByteArray)));
    connect(_terminal, SIGNAL(screenUpdated()), this, SLOT(updateBackImage()));
    connect(_terminal, SIGNAL(shouldExtendTop(int,int)),
            this, SLOT(extendTop(int,int)));
    connect(_terminal, SIGNAL(shouldExtendBottom(int,int)),
            this, SLOT(extendBottom(int,int)));
}

}   // namespace Qelly

}   // namespace UJ
