#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QLineEdit;
class QMenuBar;
class QToolBar;

namespace UJ
{

namespace Qelly
{

class SharedPreferences;
class TabWidget;

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

protected:
    void closeEvent(QCloseEvent *e);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);

signals:
    void windowShouldClose();

private:
    void buildToolBar();
    int _width;
    int _height;
    TabWidget *_tabs;
    QToolBar *_toolbar;
    QWidget *_stretch;
    QWidget *_inputFrame;
    QLineEdit *_address;
    SharedPreferences *_prefs;
    int _contentHeight;
    bool _firstRun;

public: // Setts & Getters
    inline QLineEdit *address()
    {
        return _address;
    }
    inline TabWidget *tabs()
    {
        return _tabs;
    }
    inline void setContentHeight(int height)
    {
        _contentHeight = height;
    }
};

}   // namespace Qelly

}   // namespace UJ

#endif // MAINWINDOW_H
