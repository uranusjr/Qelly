#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
class QTabBar;

namespace UJ
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

signals:

public slots:

};

}   // namespace UJ

#endif // TABWIDGET_H
