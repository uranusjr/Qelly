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
#include "View_p.h"
#include <QAction>
#include <QClipboard>
#include <QDesktopServices>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QTextCodec>
#include <QTimer>
#include <QUrl>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QUrlQuery>
#endif
#include "AbstractConnection.h"
#include "Encodings.h"
#include "PreeditTextHolder.h"
#include "SharedPreferences.h"
#include "Site.h"
#include "Terminal.h"

namespace UJ
{

namespace Qelly
{

View::View(QWidget *parent) : Widget(parent)
{
    d_ptr = new ViewPrivate(this);
    Q_D(View);

    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_KeyCompression, false);     // One key per key event
    setFixedSize(d->cellWidth * d->column, d->cellHeight * d->row);
    startTimer(QApplication::cursorFlashTime());    // NOTE: Use preferences
}

View::~View()
{
    delete d_ptr;
}

void View::timerEvent(QTimerEvent *)
{
    Q_D(View);
    d->blinkTicker = !d->blinkTicker;
    update();
}

void View::mousePressEvent(QMouseEvent *e)
{
    Q_D(View);

    setFocus(Qt::MouseFocusReason);
    if (isConnected())
    {
        d->terminal->setHasMessage(false);

        if (e->button() == Qt::LeftButton)
        {
            d->clearSelection();
            d->selectedStart = d->indexFromPoint(e->pos());

            // META + click = move the cursor
            if (e->modifiers() & UJ::ModModifier)
            {
                d->moveCursorTo(d->selectedStart / d->column,
                                d->selectedStart % d->column);
            }
        }
    }

    return Qx::Widget::mousePressEvent(e);
}

void View::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_D(View);

    if (isConnected() && !(e->modifiers() & UJ::MOD))
    {
        int r = d->selectedStart / d->column;
        int c = d->selectedStart % d->column;
        d->terminal->updateDoubleByteStateForRow(r);
        BBS::Cell *row = d->terminal->cellsAtRow(r);
        switch (row[c].attr.f.doubleByte)
        {
        case 1: // First half of double byte
            d->selectedLength = 2;
            break;
        case 2: // Second half of double byte
            d->selectedStart -= 1;
            d->selectedLength = 2;
            break;
        default:
            // Try to select the whole word/domain name on double click
            if (d->isAlphanumeric(row[c].byte))
                d->selectWordAround(r, c);
            else
                d->selectedLength = 1;
        }

        // Order redraw for selection region
        int ox = d->selectedStart % d->column;
        int oy = d->selectedStart / d->column;
        int dx = d->cellWidth * d->selectedLength;
        int dy = d->cellHeight;
        update(QRect(d->pointFromIndex(ox, oy), QSize(dx, dy)));
    }

    return Qx::Widget::mouseDoubleClickEvent(e);
}

void View::focusInEvent(QFocusEvent *)
{
    emit shouldChangeAddress(d_ptr->address);
}

void View::mouseTripleClickEvent(QMouseEvent *e)
{
    Q_D(View);

    if (isConnected() && !(e->modifiers() & UJ::MOD))
    {
        // Select the whole line on triple click
        d->selectedStart = d->selectedStart - (d->selectedStart % d->column);
        d->selectedLength = d->column;
    }

    return Qx::Widget::mouseTripleClickEvent(e);
}

void View::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(View);

    if (isConnected())
    {
        int index = d->indexFromPoint(e->pos());
        int old = d->selectedLength;
        d->selectedLength = index - d->selectedStart + 1;
        if (d->selectedLength <= 0)
            d->selectedLength--;
        if (old != d->selectedLength)
        {
            int head = d->selectedStart + old;
            if (old > 0)
                head = d->selectedStart < index ? d->selectedStart : index;
            else
                head = head < index ? head : index;
            int tail = d->selectedStart + old;
            if (old > 0)
                tail = d->selectedStart > index ? d->selectedStart : index;
            else
                tail = tail > index ? tail : index;
            update(0, head / d->column * d->cellHeight,
                   d->column * d->cellWidth, (tail - head + 1) * d->cellHeight);
        }
    }
    return Qx::Widget::mouseMoveEvent(e);
}

void View::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(View);

    if (!d->selectedLength && isConnected())
    {
        int index = d->indexFromPoint(e->pos());
        bool hasUrl = false;
        QString url = d->terminal->urlStringAt(
                    index / d->column, index % d->column, &hasUrl);
        if (hasUrl && e->button() == Qt::LeftButton
                && !(e->modifiers() & UJ::MOD))
        {
            // NOTE: Should we implement image previewer at all?
            QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
        }
    }

    return Qx::Widget::mouseReleaseEvent(e);
}

void View::keyPressEvent(QKeyEvent *e)
{
    Q_D(View);

    if (isConnected())
    {
        d->preeditHolder->hide();
        int key = e->key();
        if (key != UJ::Key_Mod)
            d->clearSelection();
        d->terminal->setHasMessage(false);
        QString text = e->text();
        if (text.isEmpty())   // Special key (up, down, etc.) or modified
        {
            Qt::KeyboardModifiers modifiers = e->modifiers();
            bool ok = false;
            switch (key)
            {
            case Qt::Key_Tab:
                emit hasBytesToSend(QByteArray(1, ASC_HT));
                break;
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Right:
            case Qt::Key_Left:
                d->handleArrowKey(key);
                break;
            case Qt::Key_PageUp:
            case Qt::Key_PageDown:
            case Qt::Key_Home:
            case Qt::Key_End:
                d->handleJumpKey(key);
                break;
            case Qt::Key_Delete:
                d->handleForwardDeleteKey();
            case 0x7f:
                //d->handleAsciiDelete();
                break;
            default:
                if (modifiers & Qt::ControlModifier)
                {
                    char c = d->characterFromKeyPress(key, modifiers, &ok);
                    if (ok)
                        emit hasBytesToSend(QByteArray(1, c));
                }
            }
        }
        else    // Normal input
        {
            emit hasBytesToSend(text.toLatin1());
        }
    }

    return Qx::Widget::keyPressEvent(e);
}

void View::inputMethodEvent(QInputMethodEvent *e)
{
    Q_D(View);

    if (isConnected())
    {
        QString commit = e->commitString();
        if (!commit.isEmpty())
            insertText(commit);

        if (e->preeditString() != d->preeditHolder->text())
        {
            d->preeditHolder->updateText(e->preeditString());
            if (d->preeditHolder->text().isEmpty())
            {
                d->preeditHolder->hide();
            }
            else
            {
                int x = d->terminal->cursorColumn();
                int y = d->terminal->cursorRow() - 1;
                d->preeditHolder->move(d->pointFromIndex(x, y));
                d->preeditHolder->show();
            }
        }
    }

    Qx::Widget::inputMethodEvent(e);
}

void View::insertText(QString &string, uint delayMs)
{
    Q_D(View);

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
                d->insertBuffer.enqueue(b);
        }
        else
        {
            ushort code;
            switch (d->terminal->connection()->site()->encoding())
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
                d->insertBuffer.enqueue(bu);
                d->insertBuffer.enqueue(bl);
            }
        }
    }

    if (!delayMs)
        emit hasBytesToSend(bytes);
    else
        d->insertTimer->start(delayMs);
}

void View::popInsertBuffer()
{
    Q_D(View);

    emit hasBytesToSend(QByteArray(1, d->insertBuffer.dequeue()));
    if (d->insertBuffer.isEmpty())
        d->insertTimer->stop();
}

void View::openUrl()
{
    QStringList urls;
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        // The user data can be a QStringList or QString. Both can be converted
        // to QStringList, so we don't need anything else
        urls = action->data().toStringList();
    }
    foreach (const QString &url, urls)
        QDesktopServices::openUrl(d_ptr->realize(url));
}

void View::google()
{
    QString queryString;
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        queryString = action->data().toString();
    if (!queryString.isEmpty())
    {
        QUrl url("http://www.google.com/search");
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        url.addEncodedQueryItem("q", QUrl::toPercentEncoding(queryString));
#else
        QUrlQuery query;
        query.addQueryItem("q", QUrl::toPercentEncoding(queryString));
        url.setQuery(query);
#endif
        QDesktopServices::openUrl(url);
    }
}

Connection::Terminal *View::terminal() const
{
    return d_ptr->terminal;
}

void View::updateScreen()
{
    Q_D(View);

    if (!isConnected())
        return;

    updateBackImage();
    int x = d->terminal->cursorColumn();
    int y = d->terminal->cursorRow();
    if (d->x != x || d->y != y)
    {
        d->displayCellAt(d->x, d->y);  // Un-draw the old cursor
        d->x = x;
        d->y = y;
    }
    d->displayCellAt(d->x, d->y);    // Draw current cursor
}

void View::updateBackImage()
{
    Q_D(View);

    for (int y = 0; y < d->row; y++)
    {
        // Background
        int start = d->column;
        for (int x = 0; x < d->column; x++)
        {
            if (d->terminal->isDiryAt(y, x))
            {
                start = x;
                while (x < d->column && d->terminal->isDiryAt(y, x))
                    x++;
                updateBackground(y, start, x);
            }
        }

        // Foreground
        updateText(y);

        for (int x = 0; x < d->column; x++)
            d->terminal->setDirtyAt(y, x, false);
    }
}

void View::updateBackground(int row, int startColumn, int endColumn)
{
    Q_D(View);

    BBS::Cell *cells = d->terminal->cellsAtRow(row);
    BBS::CellAttribute now;
    BBS::CellAttribute last = cells[startColumn].attr;
    int length = 1;

    d->painter->begin(d->backImage);
    for (int x = startColumn + 1; x <= endColumn; x++)
    {
        now = cells[x].attr;
        bool changed = now.f.bColorIndex != last.f.bColorIndex ||
                       (now.f.reversed & now.f.bright)
                            != (last.f.reversed & last.f.bright) ||
                       x == endColumn;
        if (changed)
        {
            d->painter->fillRect(
                        (x - length) * d->cellWidth, row * d->cellHeight,
                        length * d->cellWidth, d->cellHeight,
                        d->prefs->bColor(last.f.bColorIndex,
                                         last.f.reversed && last.f.bright));
            length = 1;
            last = now;
        }
        else
        {
            length++;
        }
    }

    d->painter->end();

    QRect rect(startColumn * d->cellWidth, row * d->cellHeight,
               (endColumn - startColumn) * d->cellWidth, d->cellHeight);
    update(rect);
}

void View::updateText(int row)
{
    Q_D(View);

    d->terminal->updateDoubleByteStateForRow(row);

    // Find first dirty position
    int x = 0;
    while (x < d->column && !d->terminal->isDiryAt(row, x))
        x++;
    if (x == d->column)
        return;

    int start = x;

    for (x = start; x < d->column; x++)
    {
        if (!d->terminal->isDiryAt(row, x))
            continue;
        updateText(row, x);
    }
    update((start - 1) *d->cellWidth, row * d->cellHeight,
           3 * d->cellWidth, d->cellHeight);
}

void View::updateText(int row, int x)
{
    d_ptr->updateText(row, x);
}

void View::paintEvent(QPaintEvent *e)
{
    Q_D(View);

    d->painter->begin(this);
    if (isConnected())
    {
        QRect r = e->rect();

        // Draw a portion of back image
        d->painter->drawPixmap(r.left(), r.top(), *d->backImage,
                             r.left(), r.top(), r.width(), r.height());
        d->paintBlink(r);

        // URL line
        // NOTE: Preference for URL color and width
        d->painter->setPen(QPen(QColor("orange"), 1.0));
        for (int y = r.top() / d->cellHeight;
             y <= r.bottom() / d->cellHeight; y++)
        {
            BBS::Cell *cells = d->terminal->cellsAtRow(y);
            int xEnd = r.right() / d->cellWidth + 1;
            for (int x = r.left() / d->cellWidth; x < xEnd; x++)
            {
                int start = x;
                while (start < xEnd && cells[x].attr.f.isUrl)
                    x++;
                if (start != x)
                {
                    // NOTE: Prefernce for URL y offset (the -0.5)
                    int yPos = (y + 1) * d->cellHeight - 0.5;
                    d->painter->drawLine(start *d->cellWidth, yPos,
                                       x * d->cellWidth, yPos);
                }
            }
        }

        // Cursor
        // NOTE: Preference for cursor color and shape (?)
        //       Should a non-line type cursor be implemented?
        d->painter->setPen(Qt::NoPen);
        d->painter->setBrush(QBrush(Qt::white, Qt::SolidPattern));
        d->x = d->terminal->cursorColumn();
        d->y = d->terminal->cursorRow();
        // NOTE: Prefernce for cursor y offset (the -2)
        int yPos = (d->y + 1) * d->cellHeight - 2;
        d->painter->drawRect(d->x * d->cellWidth, yPos, d->cellWidth, 2);

        // Selection
        if (d->selectedLength)
            d->paintSelection();
    }
    else
    {
        // Clear everything in the widget
        d->painter->fillRect(0, 0, width(), height(),
                             d->prefs->backgroundColor());
    }

    d->painter->end();
    return Qx::Widget::paintEvent(e);
}

void View::extendBottom(int start, int end)
{
    Q_D(View);

    int width = d->column * d->cellWidth;
    int height = (end - start + 1) * d->cellHeight;
    QPixmap m(width, height);
    d->painter->begin(&m);
    d->painter->drawPixmap(0, 0 - (start * d->cellHeight),
                           width, height, *d->backImage);
    d->painter->end();
    d->painter->begin(d->backImage);
    d->painter->drawPixmap(0, (start - 1) * d->cellHeight, width, height, m);
    d->painter->fillRect(0, end * d->cellHeight, width, d->cellHeight,
                       d->prefs->backgroundColor());
    d->painter->end();
}

void View::extendTop(int start, int end)
{
    Q_D(View);

    int width = d->column * d->cellWidth;
    int height = (end - start + 1) * d->cellHeight;
    QPixmap m(width, height);
    d->painter->begin(&m);
    d->painter->drawPixmap(0, 0 - (start * d->cellHeight),
                           width, height, *d->backImage);
    d->painter->end();
    d->painter->begin(d->backImage);
    d->painter->drawPixmap(0, (start + 1) * d->cellHeight, width, height, m);
    d->painter->fillRect(0, start * d->cellHeight, width, d->cellHeight,
                         d->prefs->backgroundColor());
    d->painter->end();
}

void View::copy()
{
    Q_D(View);

    if (!d->selectedLength)
        return;

    int start;
    int length;
    if (d->selectedLength > 0)
    {
        start = d->selectedStart;
        length = d->selectedLength;
    }
    else
    {
        start = d->selectedStart + d->selectedLength;
        length = 0 - d->selectedLength;
    }

    QMimeData *mime = new QMimeData();

    // Pure text
    QString selection = d->terminal->stringFromIndex(start, length);
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
        int x = i % d->column;
        int y = i / d->column;
        BBS::Cell &cell = d->terminal->cellsAtRow(y)[x];
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
    Q_D(View);

    const QMimeData *mime = QApplication::clipboard()->mimeData();
    QByteArray bytes = mime->data("application/x-ansi-colored-text-data");

    if (bytes.isEmpty())
        return;

    QByteArray esc;
    switch (d->terminal->connection()->site()->colorKey())
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
        d->insertBuffer.enqueue(data[i]);
    d->insertTimer->start();
}

void View::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu(this);
    d_ptr->addActionsToContextMenu(&menu);
    if (!menu.actions().isEmpty())
        menu.exec(e->globalPos());      // Execute BLOCKING menu
    return Qx::Widget::contextMenuEvent(e);
}

bool View::isConnected()
{
    Q_D(View);
    return (d->terminal != 0 && d->terminal->connection() != 0 &&
            d->terminal->connection()->isConnected());
}

void View::setTerminal(Connection::Terminal *terminal)
{
    Q_D(View);
    if (d->terminal == terminal)
        return;
    disconnect(d->terminal);
    delete d->terminal;
    d->terminal = terminal;
    if (!d->terminal)
        return;
    d->terminal->setView(this);
    connect(d->terminal, SIGNAL(dataProcessed()), SLOT(updateScreen()));
    d->terminal->connection()->connect(this, SIGNAL(hasBytesToSend(QByteArray)),
                                       SLOT(sendBytes(QByteArray)));
    connect(d->terminal, SIGNAL(screenUpdated()), SLOT(updateBackImage()));
    connect(d->terminal, SIGNAL(shouldExtendTop(int,int)),
            SLOT(extendTop(int,int)));
    connect(d->terminal, SIGNAL(shouldExtendBottom(int,int)),
            SLOT(extendBottom(int,int)));
}

void View::setAddress(const QString &address)
{
    d_ptr->address = address;
}

}   // namespace Qelly

}   // namespace UJ
