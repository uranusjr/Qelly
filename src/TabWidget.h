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
    inline QTabBar *getTabBar() const
    {
        return tabBar();
    }
    int addTab(QWidget *widget, const QIcon &icon, const QString &label);
    int addTab(QWidget *widget, const QString &label);

public slots:
    void closeTab(int index);

signals:
    void tabWillClose(QWidget *tab);

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
