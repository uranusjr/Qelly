#include "TabWidget.h"
#include <QApplication>

namespace UJ
{

namespace Qelly
{

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    int size = QApplication::font().pointSize();
    QString s;
    s.sprintf("QTabBar::tab { width: 150px; font: %dpt; }",
              static_cast<int>(size * 0.9));
    setStyleSheet(s);
    setDocumentMode(true);
    setTabsClosable(true);
    setMovable(true);
    setUsesScrollButtons(true);
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
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
    emit tabWillClose(w);
    w->deleteLater();
}

}   // namespace Qelly

}   // namespace UJ
