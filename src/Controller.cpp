/*****************************************************************************
 * Controller.cpp
 *
 * Created: 09/10 2011 by uranusjr
 *
 * Copyright 2011 uranusjr. All rights reserved.
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

#include "Controller.h"
#include <QApplication>
#include <QDesktopServices>
#include <QLineEdit>
#include <QMessageBox>
#include <QUrl>
#include "Globals.h"
#include "MainWindow.h"
#include "PreferencesWindow.h"
#include "SharedMenuBar.h"
#include "SharedPreferences.h"
#include "TabWidget.h"
#include "Telnet.h"
#include "Terminal.h"
#include "View.h"
#include "Tab.h"

namespace UJ
{

namespace Qelly
{

Controller::Controller(QObject *parent) :
    QObject(parent), _antiIdleTimer(0), _preferencesWindow(0)
{
    _window = new MainWindow();
    SharedMenuBar *menu = SharedMenuBar::sharedInstance();
    setAntiIdleTimer(SharedPreferences::sharedInstance()->isAntiIdleActive());

    connect(menu, SIGNAL(preferences()), SLOT(showPreferencesWindow()));
    connect(menu, SIGNAL(fileNewTab()), SLOT(addTab()));
    connect(menu, SIGNAL(fileOpenLocation()), SLOT(focusAddressField()));
    connect(menu, SIGNAL(fileReconnect()), SLOT(reconnect()));
    connect(menu, SIGNAL(fileCloseTab()), SLOT(closeTab()));
    connect(menu, SIGNAL(fileQuit()), SLOT(closeWindow()));
    connect(menu, SIGNAL(editCopy()), SLOT(copy()));
    connect(menu, SIGNAL(editPaste()), SLOT(paste()));
    connect(menu, SIGNAL(editPasteColor()), SLOT(pasteColor()));
    connect(menu, SIGNAL(viewAntiIdle(bool)), SLOT(toggleAntiIdle(bool)));
    connect(menu, SIGNAL(about()), SLOT(showAbout()));
    connect(menu, SIGNAL(helpVisitProjectHome()), SLOT(visitProject()));
    connect(_window, SIGNAL(reconnect()), SLOT(reconnect()));
    connect(_window, SIGNAL(windowShouldClose()), SLOT(closeWindow()));
    connect(_window, SIGNAL(newTabRequested()), SLOT(addTab()));
    connect(_window, SIGNAL(antiIdleTriggered(bool)),
            SLOT(toggleAntiIdle(bool)));
    connect(_window->address(), SIGNAL(returnPressed()),
            SLOT(onAddressReturnPressed()));
    connect(_window->tabs(), SIGNAL(tabCloseRequested(int)),
            SLOT(closeTab(int)));

    _window->show();
    addTab();
}

Controller::~Controller()
{
    _window->deleteLater();
}

void Controller::connectWithAddress(QString address)
{
    // NOTE: Search saved sites for matching address
    QString name = address;
    _window->tabs()->setTabText(_window->tabs()->currentIndex(), name);
    View *view = currentView();
    Connection::Terminal *terminal = new Connection::Terminal(view);
    Connection::AbstractConnection *connection;
    qint16 defaultPort = Connection::AbstractConnection::DefaultPort;
    if (address.startsWith("ssh://"))
    {
        address = address.section("://", 1);
        connection = new Connection::Ssh(terminal);
        terminal->setConnection(connection);
        defaultPort = Connection::Ssh::DefaultPort;
    }
    else if (address.startsWith("telnet://"))
    {
        address = address.section("://", 1);
        connection = new Connection::Telnet(terminal);
        terminal->setConnection(connection);
        defaultPort = Connection::Telnet::DefaultPort;
    }
    else
    {
        connection = new Connection::Telnet(terminal);
        terminal->setConnection(connection);
        defaultPort = Connection::Telnet::DefaultPort;
    }
    view->setTerminal(terminal);
    view->setAddress(_window->address()->text());
    view->setFocus(Qt::OtherFocusReason);
    connect(view, SIGNAL(shouldChangeAddress(const QString &)),
            this, SLOT(changeAddressField(const QString &)));

    QStringList comps = address.split(':');
    if (comps.size() == 1)
        connection->connectTo(address, defaultPort);
    else
        connection->connectTo(comps.first(), comps.last().toLong());
}

void Controller::focusAddressField()
{
    _window->address()->setFocus(Qt::ShortcutFocusReason);
}

void Controller::addTab()
{
    _window->tabs()->addTab(new Tab(new View()), "");
    _window->address()->setText(QString());
    focusAddressField();
}

void Controller::closeTab()
{
    closeTab(_window->tabs()->currentIndex());
}

void Controller::closeTab(int index)
{
    TabWidget *tabs = _window->tabs();
    View *view = viewInTab(index);
    if (view)
    {
        if (view->isConnected())
        {
            QMessageBox sure(_window);
            sure.setIcon(QMessageBox::Warning);
            sure.setText(tr("Are you sure you want to close this tab?"));
            sure.setInformativeText(
                tr("The connection is still alive. If you close this tab, the "
                   "connection will be lost. Do you want to close this tab "
                   "anyway?"));
            sure.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            sure.setWindowModality(Qt::WindowModal);
            sure.setFocus(Qt::PopupFocusReason);
            switch (sure.exec())
            {
            case QMessageBox::Ok:
                tabs->closeTab(index);
                break;
            case QMessageBox::Cancel:
                break;
            default:
                break;
            }
        }
        else
        {
            tabs->closeTab(index);
        }
    }

    // Finish up after the tab is closed
    view = viewInTab(_window->tabs()->currentIndex());
    if (view)
    {
        view->setFocus(Qt::TabFocusReason);
    }
    else
    {
        _window->address()->setFocus(Qt::TabFocusReason);
        _window->address()->setText(QString());
    }
}

void Controller::closeWindow()
{
    if (!_window->tabs()->count())
    {
        qApp->quit();
        return;
    }

    int count = 0;
    for (int i = 0; i < _window->tabs()->count(); i++)
    {
        if (viewInTab(i)->isConnected())
            count++;
    }

    if (count)
    {
        QMessageBox *sure = new QMessageBox(_window);
        sure->setIcon(QMessageBox::Warning);
        sure->setText(tr("Are you sure you want to quit Qelly?"));
        sure->setInformativeText(
            tr("There are %n tab(s) open in Qelly. Do you want to quit anyway?",
               "", count));
        sure->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        sure->setWindowModality(Qt::WindowModal);
        sure->setFocus(Qt::PopupFocusReason);

        switch (sure->exec())
        {
        case QMessageBox::Ok:
            qApp->quit();
            break;
        case QMessageBox::Cancel:
            break;
        default:
            break;
        }
        sure->deleteLater();
    }
    else
    {
        qApp->quit();
    }
}

void Controller::copy()
{
    View *view = currentView();
    if (!view || !view->isConnected())
        return;

    view->copy();
}

void Controller::paste()
{
    View *view = currentView();
    if (!view || !view->isConnected())
        return;

    view->paste();
}

void Controller::pasteColor()
{
    View *view = currentView();
    if (!view || !view->isConnected())
        return;

    view->pasteColor();
}

void Controller::reconnect()
{
    View *view = currentView();
    if (!view)
        return;
    Connection::Terminal *terminal = view->terminal();
    if (!terminal)
        return;
    Connection::AbstractConnection *connection = terminal->connection();
    if (!connection || !connection->isConnected())
        return;
    connection->reconnect();
}

void Controller::onAddressReturnPressed()
{
    QString address = _window->address()->text();
    if (!address.size())
        return;

    if (!_window->tabs()->count() || currentView()->terminal())
    {
        int newTab = _window->tabs()->addTab(new Tab(new View()), "");
        _window->tabs()->setCurrentIndex(newTab);
    }
    connectWithAddress(address);
}

void Controller::changeAddressField(const QString &address)
{
    _window->address()->setText(address);
}

void Controller::showPreferencesWindow()
{
    if (!_preferencesWindow)
    {
        _preferencesWindow = new PreferencesWindow(_window);
        _preferencesWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(_preferencesWindow, SIGNAL(displayPreferenceChanged()),
                SLOT(updateAll()));
    }
    _preferencesWindow->setAttribute(Qt::WA_ShowModal);
    _preferencesWindow->show();
}

void Controller::showAbout()
{
    // TODO: Implement me
}

void Controller::visitProject()
{
    QDesktopServices::openUrl(QUrl("https://github.com/uranusjr/Qelly"));
}

void Controller::toggleAntiIdle(bool enabled)
{
    SharedPreferences::sharedInstance()->setAntiIdleActive(enabled);
    setAntiIdleTimer(enabled);
}

void Controller::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == _antiIdleTimer)
    {
        for (int i = 0; i < _window->tabs()->count(); i++)
        {
            Connection::Terminal *terminal = viewInTab(i)->terminal();
            if (!terminal || !terminal->isConnected())
                continue;

            QDateTime now = QDateTime::currentDateTime();
            Connection::AbstractConnection *conn = terminal->connection();
            if (conn->lastTouch().secsTo(now) >= 119)
            {
                qDebug() << "sending";
                conn->sendBytes(QByteArray("\0\0\0\0\0\0", 6));
            }
        }
    }
}

void Controller::updateAll()
{
    View *view = currentView();
    if (!view)
        return;
    view->updateCellSize();

    Connection::Terminal *terminal = view->terminal();
    if(!terminal)
        return;
    terminal->setDirtyAll();
    view->updateBackImage();
    view->update();
}

View *Controller::currentView() const
{
    return static_cast<Tab *>(_window->tabs()->currentWidget())->view();
}

View *Controller::viewInTab(int index) const
{
    return static_cast<Tab *>(_window->tabs()->widget(index))->view();
}

void Controller::setAntiIdleTimer(bool enabled)
{
    if (enabled)
    {
        _antiIdleTimer = startTimer(2 * 60 * 1000);
    }
    else
    {
        killTimer(_antiIdleTimer);
        _antiIdleTimer = 0;
    }
}

}   // namespace Qelly

}   // namespace UJ
