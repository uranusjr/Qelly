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

#include "Tab.h"
#include <cmath>
#include <QResizeEvent>
#include "View.h"

namespace UJ
{

namespace Qelly
{

Tab::Tab(View *view) : QWidget(0), _view(view)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    view->setParent(this);
    view->installEventFilter(this);
}

bool Tab::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == _view)
    {
        switch (e->type())
        {
        case QEvent::Resize:
        {
            QSize sz = _view->size();
            if (sz.isValid())
            {
                setMinimumSize(sz);
                resize(sz);
                centerView(size());
            }
            break;
        }
        default:
            break;
        }
    }
    return false;
}

void Tab::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    centerView(e->size());
}

QSize Tab::sizeHint() const
{
    return _view->size();
}

void Tab::centerView(const QSize &newSize)
{
    int left = ::ceil((newSize.width() - _view->width()) / 2.0);
    int top = ::ceil((newSize.height() - _view->height()) / 2.0);
    _view->move(left, top);
}

}   // namespace Qelly

}   // namespace UJ
