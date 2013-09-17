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
