/*****************************************************************************
 * TripleClickableWidget.h
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

/* This class is inspired by                                                 *
 * https://qt.gitorious.org/qt/qtdeclarative/merge_requests/6                *
 * which should be available in Qt 4.8.0 and later.                          *
 * But the function the merge request proposed only applies to text input    *
 * widgets, whereas this class directly extends QWidget.                     */

#ifndef UJQXWIDGET_H
#define UJQXWIDGET_H

#include <QWidget>
class QTimer;

namespace UJ
{

namespace Qx
{

namespace Event
{
    enum Type
    {
        MouseButtonTplClick = 1006
    };
}   // namespace Event

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseTripleClickEvent(QMouseEvent *);

signals:
    void mouseTripleClicked(QMouseEvent *e);

private slots:
    void tripleClickTimedout();

private:
    QTimer *_tripleClickTimer;
    bool _pendingTripleClick;

};

}   // namespace Qx

}   // namespace UJ

#endif // UJQXWIDGET_H
