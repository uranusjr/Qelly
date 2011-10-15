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
    inline QSize fixedSize()
    {
        return QSize(_width, _height);
    }

protected:
    void closeEvent(QCloseEvent *e);

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

public: // Setts & Getters
    inline QLineEdit *address()
    {
        return _address;
    }
    inline TabWidget *tabs()
    {
        return _tabs;
    }

};

}   // namespace Qelly

}   // namespace UJ

#endif // MAINWINDOW_H
