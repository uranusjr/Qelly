#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QMenuBar;
class QToolBar;

namespace UJ
{

class TabWidget;

namespace Qelly
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    static inline MainWindow *sharedInstance()
    {
        static MainWindow *g = new MainWindow();
        return g;
    }

private:
    void buildToolBar();
    TabWidget *_tabs;
    QToolBar *_toolbar;
    QWidget *_stretch;
    QWidget *_inputFrame;
};

}   // namespace Qelly

}   // namespace UJ

#endif // MAINWINDOW_H
