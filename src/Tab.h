/*****************************************************************************
 * Tab.h
 *
 * Created: 01/10 2013 by uranusjr
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

#ifndef TAB_H
#define TAB_H

#include <QWidget>

namespace UJ
{

namespace Qelly
{

class View;

class Tab : public QWidget
{
    Q_OBJECT

public:
    explicit Tab(View *view);
    inline View *view() { return _view; }

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual QSize sizeHint() const;

private:
    View *_view;
    void centerView(const QSize &newSize);
};

}   // namespace Qelly

}   // namespace UJ

#endif // TAB_H
