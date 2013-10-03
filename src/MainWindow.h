/*****************************************************************************
 * MainWindow.h
 *
 * Created: 05/10 2011 by uranusjr
 *
 * Copyright 2013 uranusjr. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/

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
    static inline MainWindow *sharedInstance()
    {
        static MainWindow *g = new MainWindow();
        return g;
    }

protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual bool event(QEvent *e);

signals:
    void windowShouldClose();
    void newTabRequested();
    void reconnect();
    void antiIdleTriggered(bool);
    void showHiddenTextTriggered(bool);
    void emoticonViewerShouldOpen();
    void siteManageShouldOpen();

private:
    void buildToolBar();
    TabWidget *_tabs;
    QToolBar *_toolbar;
    QWidget *_inputFrame;
    QLineEdit *_address;
    SharedPreferences *_prefs;

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
