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
#include <QApplication>
#include <QByteArray>
#include <QClipboard>
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
#include "PreeditTextHolder.h"
#include "SharedPreferences.h"
#include "Site.h"
#include "Terminal.h"
#include "UJCommonDefs.h"
#include <QDebug>

namespace UJ
{

namespace Qelly
{

View::View(QWidget *parent) : Widget(parent)
{
    _address = "";
    _backImage = 0;
    _terminal = 0;
    _blinkTicker = false;
    _prefs = SharedPreferences::sharedInstance();
    _painter = new QPainter();
    _preeditHolder = new PreeditTextHolder(this);
    buildInfo();
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_KeyCompression, false);     // One key per key event
    setFixedSize(_cellWidth * _column, _cellHeight * _row);
    startTimer(QApplication::cursorFlashTime());    // NOTE: Use preferences
}

View::~View()
{
    delete _backImage;
    delete _painter;
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

QPoint View::pointFromIndex(int x, int y)
{
    if (x < 0)
        x = 0;
    else if (x > _column)
        x = _column;

    if (y < 0)
        y = 0;
    else if (y > _row)
        y = _row;

    return QPoint(x * _cellWidth, y * _cellHeight);
}

void View::timerEvent(QTimerEvent *)
{
    _blinkTicker = !_blinkTicker;
    update();
}

void View::mousePressEvent(QMouseEvent *e)
{
    setFocus(Qt::MouseFocusReason);
    if (isConnected())
    {
        terminal()->setHasMessage(false);

        // META + click = move the cursor
        if (e->modifiers() & UJ::MOD)
            moveCursorTo(_selectedStart / _column, _selectedStart % _column);

        clearSelection();
        _selectedStart = indexFromPoint(e->pos());
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
        switch (row[c].attr.f.doubleByte)
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

void View::focusInEvent(QFocusEvent *)
{
    emit shouldChangeAddress(_address);
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
            _selectedLength--;
        if (old != _selectedLength)
        {
            int head = _selectedStart + old;
            if (old > 0)
                head = _selectedStart < index ? _selectedStart : index;
            else
                head = head < index ? head : index;
            int tail = _selectedStart + old;
            if (old > 0)
                tail = _selectedStart > index ? _selectedStart : index;
            else
                tail = tail > index ? tail : index;
            update(0, head / _column * _cellHeight,
                   _column * _cellWidth, (tail - head + 1) * _cellHeight);
        }
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
        _preeditHolder->hide();
        int key = e->key();
        if (key != UJ::Key_Mod)
            clearSelection();
        terminal()->setHasMessage(false);
        QString text = e->text();
        if (text.isEmpty())   // Special key (up, down, etc.) or modified
        {
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

QVariant View::inputMethodQuery(Qt::InputMethodQuery q) const
{
    if (q == Qt::ImCursorPosition)
    {
        int x = terminal()->cursorColumn();
        int y = terminal()->cursorRow();
        return mapToGlobal(QPoint(x * _cellWidth + _preeditHolder->textWidth(),
                             y * _cellHeight));
    }

    return QWidget::inputMethodQuery(q);
}

void View::inputMethodEvent(QInputMethodEvent *e)
{
    if (isConnected())
    {
        QString commit = e->commitString();
        if (!commit.isEmpty())
            insertText(commit);

        if (e->preeditString() != _preeditHolder->text())
        {
            _preeditHolder->updateText(e->preeditString());
            if (_preeditHolder->text().isEmpty())
            {
                _preeditHolder->hide();
            }
            else
            {
                int x = terminal()->cursorColumn();
                int y = terminal()->cursorRow() - 1;
                _preeditHolder->move(pointFromIndex(x, y));
                _preeditHolder->show();
            }
        }
    }

    Qx::Widget::inputMethodEvent(e);
}

void View::insertText(QString &string, uint delayMs)
{
    QByteArray bytes;
    QString &s = string.replace('\n', '\r');
    for (int i = 0; i < s.size(); i++)
    {
        QChar c = s[i];
        if (c < QChar('\x7f'))
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
                code = 0;
                break;
            }
            uchar bu = code / 0x100;
            uchar bl = code % 0x100;
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
        int start = _selectedLength > 0 ?
                    _selectedStart : _selectedStart + _selectedLength - 1;
        int length = _selectedLength > 0 ?
                    _selectedLength : -(_selectedLength - 1);
        int startY = start / _column;
        int endY = (start + length) / _column;
        if (startY == endY)
        {
            update(start % _column * _cellWidth, startY * _cellHeight,
                   length * _cellWidth, _cellHeight);
        }
        else
        {
            update(0, startY * _cellHeight,
                   _column * _cellWidth, (endY - startY + 1) * _cellHeight);
        }
        _selectedLength = 0;
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
        displayCellAt(_x, _y);  // Un-draw the old cursor
        _x = x;
        _y = y;
    }
    displayCellAt(_x, _y);    // Draw current cursor
}

void View::updateBackImage()
{
    for (int y = 0; y < _row; y++)
    {
        // Background
        int start = _column;
        for (int x = 0; x < _column; x++)
        {
            if (terminal()->isDiryAt(y, x))
            {
                start = x;
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

    _painter->begin(_backImage);
    for (int x = startColumn + 1; x <= endColumn; x++)
    {
        now = cells[x].attr;
        bool changed = now.f.bColorIndex != last.f.bColorIndex ||
                       (now.f.reversed & now.f.bright)
                            != (last.f.reversed & last.f.bright) ||
                       x == endColumn;
        if (changed)
        {
            _painter->fillRect((x - length) * _cellWidth, row * _cellHeight,
                               length * _cellWidth, _cellHeight,
                               _prefs->bColor(last.f.bColorIndex,
                                              last.f.reversed &&
                                                            last.f.bright));
            length = 1;
            last = now;
        }
        else
        {
            length++;
        }
    }

    _painter->end();

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
    update((start - 1) *_cellWidth, row * _cellHeight,
           3 * _cellWidth, _cellHeight);
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
        _painter->begin(_backImage);
        _painter->setFont(sglFont);
        _painter->setPen(_prefs->fColor(attr.f.fColorIndex, attr.f.bright));
        code = cells[x].byte ? cells[x].byte : ' ';
        _painter->drawText(x * _cellWidth + sglPadLeft,
                           (row + 1) * _cellHeight - sglPadBott,
                           QChar(code));
        _painter->end();
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
                _painter->begin(_backImage);
                _painter->setFont(dblFont);
                _painter->setPen(_prefs->fColor(attr.f.fColorIndex,
                                                attr.f.bright));
                _painter->drawText((x - 1) * _cellWidth + dblPadLeft,
                                   (row + 1) * _cellHeight - dblPadBott,
                                   QChar(code));
                _painter->end();
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
    int xs[9] = {column * w, (column + 1) * w, (column + 2) * w,
                 column * w, (column + 1) * w, (column + 2) * w,
                 column * w, (column + 1) * w, (column + 2) * w};
    int ys[9] = {row * h, row * h, row * h,
                 row * h + h/2, row * h + h/2, row * h + h/2,
                 (row + 1) * h, (row + 1) * h, (row + 1) * h};
    _painter->begin(_backImage);
    _painter->setPen(Qt::NoPen);
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
        _painter->fillRect(xs[0], ys[6] - h * (code - 0x2580) / 8,
                           w, h * (code - 0x2580) / 8,
                           _prefs->fColor(left.f.fColorIndex, left.f.bright));
        _painter->fillRect(xs[1], ys[7] - h * (code - 0x2580) / 8,
                           w, h * (code - 0x2580) / 8,
                           _prefs->fColor(right.f.fColorIndex, right.f.bright));
        break;
    case 0x2589:    // ▉ Left seven eights block
    case 0x258a:    // ▊ Left three quarters block
    case 0x258b:    // ▋ Left five eighths block
        _painter->fillRect(xs[0], ys[0], w, h,
                           _prefs->fColor(left.f.fColorIndex, left.f.bright));
        _painter->fillRect(xs[1], ys[1], w * (0x258c - code) / 8, h,
                           _prefs->fColor(right.f.fColorIndex, right.f.bright));
        break;
    case 0x258c:    // ▌ Left half block
    case 0x258d:    // ▍ Left three eighths block
    case 0x258e:    // ▎ Left one quarter block
    case 0x258f:    // ▏ Left one eighth block
        _painter->fillRect(xs[0], ys[0], w * (0x2590 - code) / 8, h,
                           _prefs->fColor(left.f.fColorIndex, left.f.bright));
        break;
    case 0x25e2:    // ◢ Black lower right triangle
        //_painter->setPen(Qt::SolidLine);
        _painter->setBrush(QBrush(_prefs->fColor(left.f.fColorIndex,
                                                 left.f.bright),
                                Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[7], ys[7]);
        points[2] = QPoint(xs[6], ys[6]);
        _painter->drawPolygon(points, 3);
        _painter->setBrush(QBrush(_prefs->fColor(right.f.fColorIndex,
                                                 right.f.bright),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[8], ys[8]);
        points[3] = QPoint(xs[2], ys[2]);
        _painter->drawPolygon(points, 4);
        break;
    case 0x25e3:    // ◣ Black lower left triangle
        _painter->setBrush(QBrush(_prefs->fColor(left.f.fColorIndex,
                                                 left.f.bright),
                                Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[7], ys[7]);
        points[2] = QPoint(xs[6], ys[6]);
        points[3] = QPoint(xs[0], ys[0]);
        _painter->drawPolygon(points, 4);
        _painter->setBrush(QBrush(_prefs->fColor(right.f.fColorIndex,
                                                 right.f.bright),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[8], ys[8]);
        _painter->drawPolygon(points, 3);
        break;
    case 0x25e4:    // ◤ Black upper left triangle
        _painter->setBrush(QBrush(_prefs->fColor(left.f.fColorIndex,
                                                 left.f.bright),
                                Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[1], ys[1]);
        points[2] = QPoint(xs[0], ys[0]);
        points[3] = QPoint(xs[6], ys[6]);
        _painter->drawPolygon(points, 4);
        _painter->setBrush(QBrush(_prefs->fColor(right.f.fColorIndex,
                                                 right.f.bright),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[2], ys[2]);
        _painter->drawPolygon(points, 3);
        break;
    case 0x25e5:    // ◥ Black upper right triangle
        _painter->setBrush(QBrush(_prefs->fColor(left.f.fColorIndex,
                                                 left.f.bright),
                                Qt::SolidPattern));
        points[0] = QPoint(xs[4], ys[4]);
        points[1] = QPoint(xs[1], ys[1]);
        points[2] = QPoint(xs[0], ys[0]);
        _painter->drawPolygon(points, 3);
        _painter->setBrush(QBrush(_prefs->fColor(right.f.fColorIndex,
                                                 right.f.bright),
                                Qt::SolidPattern));
        points[2] = QPoint(xs[2], ys[2]);
        points[3] = QPoint(xs[8], ys[8]);
        _painter->drawPolygon(points, 4);
        break;
    case 0x25fc:    // ◼ Black medium square    // paint as a full block
        _painter->fillRect(xs[0], ys[0], w, h,
                           _prefs->fColor(left.f.fColorIndex, left.f.bright));
        _painter->fillRect(xs[1], ys[1], w, h,
                           _prefs->fColor(right.f.fColorIndex, right.f.bright));
        break;
    default:
        break;
    }
    _painter->end();
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
    _painter->begin(&lp);
    _painter->setFont(dblFont);
    _painter->setPen(_prefs->fColor(left.f.fColorIndex, left.f.bright));
    _painter->drawText(dblPadLeft, _cellHeight - dblPadBottom, QChar(code));
    _painter->end();

    // Right side
    QPixmap rp(_cellWidth, _cellHeight);
    rp.fill(_prefs->bColor(right.f.bColorIndex));
    _painter->begin(&rp);
    _painter->setFont(dblFont);
    _painter->setPen(_prefs->fColor(right.f.fColorIndex, right.f.bright));
    _painter->drawText(dblPadLeft - _cellWidth, _cellHeight - dblPadBottom,
                       QChar(code));
    _painter->end();

    // Draw the left half of left side, right half of the right side
    _painter->begin(_backImage);
    _painter->setBackgroundMode(Qt::TransparentMode);
    _painter->drawPixmap(column * _cellWidth, row * _cellHeight, lp);
    _painter->drawPixmap((column + 1) * _cellWidth, row * _cellHeight, rp);
    _painter->end();
}

void View::paintEvent(QPaintEvent *e)
{
    _painter->begin(this);
    if (isConnected())
    {
        QRect r = e->rect();

        // Draw a portion of back image
        _painter->drawPixmap(r.left(), r.top(), *_backImage,
                             r.left(), r.top(), r.width(), r.height());
        paintBlink(r);

        // URL line
        // NOTE: Preference for URL color and width
        _painter->setPen(QPen(QColor("orange"), 1.0));
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
                    _painter->drawLine(start *_cellWidth, yPos,
                                       x * _cellWidth, yPos);
                }
            }
        }

        // Cursor
        // NOTE: Preference for cursor color and shape (?)
        //       Should a non-line type cursor be implemented?
        _painter->setPen(Qt::NoPen);
        _painter->setBrush(QBrush(Qt::white, Qt::SolidPattern));
        _x = terminal()->cursorColumn();
        _y = terminal()->cursorRow();
        // NOTE: Prefernce for cursor y offset (the -2)
        int yPos = (_y + 1) * _cellHeight - 2;
        _painter->drawRect(_x * _cellWidth, yPos, _cellWidth, 2);

        // Selection
        if (_selectedLength)
            paintSelection();
    }
    else
    {
        // Clear everything in the widget
        _painter->fillRect(0, 0, width(), height(), _prefs->backgroundColor());
    }

    _painter->end();
    return Qx::Widget::paintEvent(e);
}

void View::paintBlink(QRect &r)
{
    if (!isConnected() || !_blinkTicker)
        return;

    for (int y = r.top() / _cellHeight; y <= r.bottom() / _cellHeight; y++)
    {
        BBS::Cell *cells = terminal()->cellsAtRow(y);
        for (int x = r.left() / _cellWidth; x < r.right() / _cellWidth + 1; x++)
        {
            BBS::CellAttribute &a = cells[x].attr;
            if (!a.f.blinking)
                continue;
            int colorIndex = a.f.reversed ? a.f.fColorIndex : a.f.bColorIndex;
            bool bright = a.f.reversed ? a.f.bright : false;
            _painter->setPen(Qt::NoPen);
            _painter->setBrush(QBrush(_prefs->bColor(colorIndex, bright)));
            _painter->drawRect(x * _cellWidth, y * _cellHeight,
                               _cellWidth, _cellHeight);
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

    Qt::BGMode bgm = _painter->backgroundMode();
    QPen p = _painter->pen();
    _painter->setPen(Qt::NoPen);
    _painter->setBackgroundMode(Qt::TransparentMode);
    _painter->setBrush(QBrush(QColor(153, 229, 153, 102), Qt::SolidPattern));
    while (len > 0)
    {
        if (x + len < _column)
        {
            _painter->drawRect(x * _cellWidth, y * _cellHeight,
                               len * _cellWidth, _cellHeight);
            len = 0;
        }
        else
        {
            _painter->drawRect(x * _cellWidth, y * _cellHeight,
                               (_column - x) * _cellWidth, _cellHeight);
            len -= _column - x;
        }
        x = 0;
        y++;
    }
    _painter->setPen(p);
    _painter->setBackgroundMode(bgm);
}

void View::refreshHiddenRegion()
{
}

void View::extendBottom(int start, int end)
{
    int width = _column * _cellWidth;
    int height = (end - start + 1) * _cellHeight;
    QPixmap m(width, height);
    _painter->begin(&m);
    _painter->drawPixmap(0, -(start * _cellHeight), width, height, *_backImage);
    _painter->end();
    _painter->begin(_backImage);
    _painter->drawPixmap(0, (start - 1) * _cellHeight, width, height, m);
    _painter->fillRect(0, end * _cellHeight, width, _cellHeight,
                       _prefs->backgroundColor());
    _painter->end();
}

void View::extendTop(int start, int end)
{
    int width = _column * _cellWidth;
    int height = (end - start + 1) * _cellHeight;
    QPixmap m(width, height);
    _painter->begin(&m);
    _painter->drawPixmap(0, -(start * _cellHeight), width, height, *_backImage);
    _painter->end();
    _painter->begin(_backImage);
    _painter->drawPixmap(0, (start + 1) * _cellHeight, width, height, m);
    _painter->fillRect(0, start * _cellHeight, width, _cellHeight,
                       _prefs->backgroundColor());
    _painter->end();
}

void View::copy()
{
    if (!_selectedLength)
        return;

    int start;
    int length;
    if (_selectedLength > 0)
    {
        start = _selectedStart;
        length = _selectedLength;
    }
    else
    {
        start = _selectedStart + _selectedLength;
        length = -(_selectedLength);
    }

    QMimeData *mime = new QMimeData();

    // Pure text
    QString selection = terminal()->stringFromIndex(start, length);
    mime->setText(selection);

    // Color copy
    BBS::CellAttribute cleared;
    cleared.f.bColorIndex = 9;  // NOTE: Use global preferences
    cleared.f.fColorIndex = 7;
    cleared.f.blinking = 0;
    cleared.f.bright = 0;
    cleared.f.underlined = 0;
    cleared.f.reversed = 0;

    QByteArray data;
    for (int i = start; i < start + length; i++)
    {
        int x = i % _column;
        int y = i / _column;
        BBS::Cell &cell = terminal()->cellsAtRow(y)[x];
        if ((x == 0) && (i != start))   // newline
        {
            data.append('\r');
            data.append(cleared.f.bColorIndex);
            data.append(cleared.f.fColorIndex);
            data.append(cleared.f.blinking);
            data.append(cleared.f.bright);
            data.append(cleared.f.underlined);
            data.append(cleared.f.reversed);
        }
        data.append(cell.byte);
        data.append(cell.attr.f.bColorIndex);
        data.append(cell.attr.f.fColorIndex);
        data.append(cell.attr.f.blinking);
        data.append(cell.attr.f.bright);
        data.append(cell.attr.f.underlined);
        data.append(cell.attr.f.reversed);
    }
    mime->setData("application/x-ansi-colored-text-data", data);

    QApplication::clipboard()->setMimeData(mime);
}

void View::paste()
{
    const QMimeData *data = QApplication::clipboard()->mimeData();
    if (data->hasText())
    {
        QString text = data->text();
        insertText(text, 1);
    }
}

void View::pasteColor()
{
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    QByteArray bytes = mime->data("application/x-ansi-colored-text-data");

    if (bytes.isEmpty())
        return;

    QByteArray esc;
    switch (terminal()->connection()->site()->colorKey())
    {
    case BBS::ColorKeyCtrlU:
        esc.append('\x15');
        break;
    case BBS::ColorKeyDoubleEsc:
        esc.append('\x1b');
        esc.append('\x1b');
        break;
    default:
        esc.append('\x1b');
        break;
    }

    BBS::CellAttribute cleared;
    cleared.f.bColorIndex = 9;  // NOTE: Use global preferences
    cleared.f.fColorIndex = 7;
    cleared.f.blinking = 0;
    cleared.f.bright = 0;
    cleared.f.underlined = 0;
    cleared.f.reversed = 0;

    int space = 0;
    BBS::CellAttribute before = cleared;
    QByteArray data;
    for (int i = 0; i < bytes.size();)
    {
        BBS::Cell cell;
        cell.byte = bytes[i++];
        cell.attr.f.bColorIndex = bytes[i++];
        cell.attr.f.fColorIndex = bytes[i++];
        cell.attr.f.blinking = bytes[i++];
        cell.attr.f.bright = bytes[i++];
        cell.attr.f.underlined = bytes[i++];
        cell.attr.f.reversed = bytes[i++];

        // Identical to the previous one
        if ((cell.attr.f.bColorIndex == before.f.bColorIndex) &&
            (cell.attr.f.fColorIndex == before.f.fColorIndex) &&
            (cell.attr.f.blinking == before.f.blinking) &&
            (cell.attr.f.bright == before.f.bright) &&
            (cell.attr.f.underlined == before.f.underlined) &&
            (cell.attr.f.reversed == before.f.reversed))
        {
            if ((cell.byte == ' ' || cell.byte == '\0') &&
                    !cell.attr.f.doubleByte)
            {
                space++;
            }
            else
            {
                for (int j = 0; j < space; j++)
                    data.append(' ');
                data.append(cell.byte);
                space = 0;
            }
            continue;
        }

        before = cell.attr;
        for (int j = 0; j < space; j++)
            data.append(' ');
        space = 0;

        if ((cell.attr.f.bColorIndex == cleared.f.bColorIndex) &&
            (cell.attr.f.fColorIndex == cleared.f.fColorIndex) &&
            (cell.attr.f.blinking == cleared.f.blinking) &&
            (cell.attr.f.bright == cleared.f.bright) &&
            (cell.attr.f.underlined == cleared.f.underlined) &&
            (cell.attr.f.reversed == cleared.f.reversed))
        {
            data.append(esc);
            data.append("[m");
            data.append(cell.byte);
        }
        else
        {
            data.append(esc);
            data.append("[0");
            if (cell.attr.f.bright)
                data.append(";1");
            if (cell.attr.f.blinking)
                data.append(";5");
            if (cell.attr.f.underlined)
                data.append(";4");
            if (cell.attr.f.reversed)
                data.append(";7");
            data.append(";3");
            data.append('0' + cell.attr.f.fColorIndex);
            data.append(";4");
            data.append('0' + cell.attr.f.bColorIndex);
            data.append('m');
            data.append(cell.byte);
        }
    }

    data.append(esc);
    data.append("[m");

    for (int i = 0; i < data.size(); i++)
        _insertBuffer.enqueue(data[i]);
    _insertTimer->start();
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
