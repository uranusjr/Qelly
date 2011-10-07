/*****************************************************************************
 * TripleClickableWidget.cpp
 *
 * Created: 07/10 2011 by uranusjr
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

#include "UJQxWidget.h"
#include <QApplication>
#include <QMouseEvent>
#include <QTimer>

namespace UJ
{

namespace Qx
{

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    _tripleClickTimer = new QTimer(this);
    _tripleClickTimer->setSingleShot(true);
    connect(_tripleClickTimer, SIGNAL(timeout()),
            this, SLOT(tripleClickTimedout()));
    _pendingTripleClick = false;
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    if (e->type() == QEvent::MouseButtonDblClick && _pendingTripleClick)
    {
        _tripleClickTimer->stop();
        _pendingTripleClick = false;
        QMouseEvent event(static_cast<QEvent::Type>(
                              static_cast<int>(Event::MouseButtonTplClick)),
                          e->pos(), e->globalPos(), e->button(), e->buttons(),
                          e->modifiers());
        mouseTripleClickEvent(&event);
    }
    return QWidget::mousePressEvent(e);
}

void Widget::mouseDoubleClickEvent(QMouseEvent *e)
{
    _pendingTripleClick = true;
    _tripleClickTimer->start(QApplication::doubleClickInterval());
    return QWidget::mouseDoubleClickEvent(e);
}

void Widget::mouseTripleClickEvent(QMouseEvent *)
{
}

void Widget::tripleClickTimedout()
{
    _pendingTripleClick = false;
}

}   // namespace Qx

}   // namespace UJ
