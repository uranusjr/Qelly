#include "TabWidget.h"
#include <QTabBar>
#include <QTextStream>
#include "View.h"

namespace UJ
{

namespace Qelly
{

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    int size = font().pointSize();
    _tabBarHeight = fontMetrics().height() * 1.5;
    QString s;
    QTextStream stream(&s);
    stream << "QTabBar::tab {"
           << "width: 150px;"
           << "font-size:" << size * 0.9 << "pt;"
           << "}";
    setStyleSheet(s);
    tabBar()->setAutoFillBackground(true);
    QPalette p;
    p.setBrush(QPalette::Window, QBrush(Qt::black));
    setPalette(p);
    setAutoFillBackground(true);
    setDocumentMode(true);
    setTabsClosable(true);
    setMovable(true);
    setUsesScrollButtons(true);

    connect(tabBar(), SIGNAL(tabMoved(int,int)), SLOT(onTabMoved(int,int)));
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
    refreshTabText(index);
    w->deleteLater();
}

void TabWidget::onTabMoved(int from, int to)
{
    int start = from;
    int end = to;
    if (start > end)
    {
        int t = end;
        end = start;
        start = t;
    }
    refreshTabText(start, end + 1);
}

void TabWidget::refreshTabText(int start, int end)
{
    if (end < start)
        end = count();
    for (int i = start; i < end; i++)
        setTabText(i, tabText(i));
}

}   // namespace Qelly

}   // namespace UJ
