#include "TabWidget.h"
#include <QApplication>
#include "View.h"

namespace UJ
{

namespace Qelly
{

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    int size = QApplication::font().pointSize();
    _tabBarHeight = QApplication::fontMetrics().height() * 1.5;
    QString s;
    s.sprintf("QTabBar::tab { width: 150px; font: %dpt; }",
              static_cast<int>(size * 0.9));
    setStyleSheet(s);
    setDocumentMode(true);
    setTabsClosable(true);
    setMovable(true);
    setUsesScrollButtons(true);
}

int TabWidget::addTab(QWidget *widget, const QIcon &icon, const QString &label)
{
    setUpdatesEnabled(false);
    widget->setParent(this);
    int result = QTabWidget::addTab(widget, icon, label);
    setTabText(result, label);
    setCurrentIndex(result);
    setUpdatesEnabled(true);
    return result;
}

int TabWidget::addTab(QWidget *widget, const QString &label)
{
    setUpdatesEnabled(false);
    widget->setParent(this);
    int result = QTabWidget::addTab(widget, label);
    setTabText(result, label);
    setCurrentIndex(result);
    setUpdatesEnabled(true);
    return result;
}

void TabWidget::closeTab(int index)
{
    QWidget *w = widget(index);
    removeTab(index);
    for (int i = index; i < count(); i++)
        setTabText(i, tabText(i));
    w->deleteLater();
}

}   // namespace Qelly

}   // namespace UJ
