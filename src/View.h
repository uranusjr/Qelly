/*****************************************************************************
 * View.h
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

#ifndef VIEW_H
#define VIEW_H

#include "UJQxWidget.h"
#include <QtGlobal>
#include "Globals.h"

namespace UJ
{

namespace Connection
{
class Terminal;
}

namespace Qelly
{

class ViewPrivate;

class View : public Qx::Widget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(View)

public:
    explicit View(QWidget *parent = 0);
    virtual ~View();
    bool needBlinking();
    bool isConnected();

public slots:
    void updateScreen();
    void updateBackImage();
    void updateBackground(int row, int startColumn, int endColumn);
    void updateText(int row);
    void updateText(int row, int x);
    void extendBottom(int start, int end);
    void extendTop(int start, int end);
    void insertText(const QString &string, uint delayMs = 0);
    void copy();
    void paste();
    void pasteColor();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mouseTripleClickEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void inputMethodEvent(QInputMethodEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void focusInEvent(QFocusEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual bool focusNextPrevChild(bool);

signals:
    void hasBytesToSend(QByteArray bytes);
    void shouldChangeAddress(const QString &address);

private slots:
    void commitFromPreeditHolder(QInputMethodEvent *e);
    void clearPreeditHolder();
    void popInsertBuffer();
    void openUrl();
    void google();

private:
    ViewPrivate *d_ptr;

public: // Setters & Getters
    Connection::Terminal *terminal() const;
    void setTerminal(Connection::Terminal *terminal);
    void setAddress(const QString &address);
};

}   // namespace Qelly

}   // namespace UJ

#endif // VIEW_H
