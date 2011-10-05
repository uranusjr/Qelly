#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
class QMenuBar;

namespace UJ
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    static inline MainWindow *sharedInstance()
    {
        static MainWindow *g = new MainWindow();
        return g;
    }

private:
    void buildCentralWidget();
    void buildSharedMenu();
};

}   // namespace UJ

#endif // MAINWINDOW_H
