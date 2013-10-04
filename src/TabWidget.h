/*****************************************************************************
 * TabWidget.h
 *
 * Created: 05/10 2011 by uranusjr
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

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
class QTabBar;

namespace UJ
{

namespace Qelly
{

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabWidget(QWidget *parent = 0);
    int addTab(QWidget *widget, const QIcon &icon, const QString &label);
    int addTab(QWidget *widget, const QString &label);

public slots:
    void closeTab(int index);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void onTabMoved(int from, int to);
    void refreshTabText(int start = 0, int end = -1);
    void goToTabForKey(int key);

private:
    void preAddTab(QWidget *w);
    void postAddTab(int index, const QString &label);

public: // Setters & Getters
    inline QString tabText(int index) const
    {
        return QTabWidget::tabText(index).section(' ', 1);
    }
    inline void setTabText(int index, const QString &text)
    {
        QTabWidget::setTabText(index, QString::number(index + 1) + ". " + text);
    }
};

}   // namespace Qelly

}   // namespace UJ

#endif // TABWIDGET_H
