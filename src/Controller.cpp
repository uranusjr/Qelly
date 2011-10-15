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
#include <QLineEdit>
#include <QMessageBox>
#include "MainWindow.h"
#include "SharedMenuBar.h"
#include "TabWidget.h"
#include "Telnet.h"
#include "Terminal.h"
#include "View.h"
#include <QDebug>

namespace UJ
{

namespace Qelly
{

Controller::Controller(QObject *parent) : QObject(parent)
{
    _window = new MainWindow();
    SharedMenuBar *menu = SharedMenuBar::sharedInstance();
    connect(menu, SIGNAL(fileNewTab()), this, SLOT(addTab()));
    connect(menu, SIGNAL(fileOpenLocation()), this, SLOT(focusAddressField()));
    connect(menu, SIGNAL(fileCloseTab()), this, SLOT(closeTab()));
    connect(_window->address(), SIGNAL(returnPressed()),
            this, SLOT(onAddressReturnPressed()));
    connect(_window, SIGNAL(windowShouldClose()), this, SLOT(closeWindow()));
    _window->show();
    _window->setFixedSize(_window->fixedSize());
    _window->address()->setFocus(Qt::ActiveWindowFocusReason);
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
    Connection::Terminal *t = new Connection::Terminal(view);
    Connection::AbstractConnection *connection;
    if (address.startsWith("ssh://"))
        return; // NOTE: Handle ssh
    else if (address.startsWith("telnet://"))
    {
        address = address.section("://", 1);
        connection = new Connection::Telnet(t);
        t->setConnection(connection);
    }
    else
    {
        connection = new Connection::Telnet(t);
        t->setConnection(connection);
    }
    view->setTerminal(t);
    view->setAddress(_window->address()->text());
    view->setFocus(Qt::OtherFocusReason);
    connect(view, SIGNAL(shouldChangeAddress(QString &)),
            this, SLOT(changeAddressField(QString &)));
    connection->connectTo(address, 23);
}

void Controller::addTab()
{
    _window->tabs()->addTab(new View(), "");
    _window->address()->setText(QString());
    _window->address()->setFocus(Qt::ShortcutFocusReason);
}

void Controller::focusAddressField()
{
    _window->address()->setFocus(Qt::ShortcutFocusReason);
}

void Controller::closeTab()
{
    TabWidget *tabs = _window->tabs();
    if (currentView()->isConnected())
    {
        QMessageBox *sure = new QMessageBox(_window);
        sure->setIcon(QMessageBox::Warning);
        sure->setText(tr("Are you sure you want to close this tab?"));
        sure->setInformativeText(
            tr("The connection is still alive. If you close this tab, the "
               "connection will be lost. Do you want to close this tab "
               "anyway?"));
        sure->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        sure->setWindowModality(Qt::WindowModal);
        sure->setFocus(Qt::PopupFocusReason);
        switch (sure->exec())
        {
        case QMessageBox::Ok:
            tabs->closeTab(tabs->currentIndex());
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
        if (tabs->count())
            tabs->closeTab(tabs->currentIndex());
    }

    if (currentView())
    {
        currentView()->setFocus(Qt::TabFocusReason);
    }
    else
    {
        _window->address()->setFocus(Qt::TabFocusReason);
        _window->address()->setText(QString());
    }
}

void Controller::closeWindow()
{
    int count = 0;
    for (int i = 0; i < _window->tabs()->count(); i++)
    {
        if (static_cast<View *>(_window->tabs()->widget(i))->isConnected())
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

void Controller::onAddressReturnPressed()
{
    QString address = _window->address()->text();
    if (!address.size())
        return;

    if (!_window->tabs()->count() ||
            static_cast<View *>(_window->tabs()->currentWidget())->terminal())
    {
        int newTab = _window->tabs()->addTab(new View(), "");
        _window->tabs()->setCurrentIndex(newTab);
    }
    connectWithAddress(address);
}

void Controller::changeAddressField(QString &address)
{
    _window->address()->setText(address);
}

View *Controller::currentView() const
{
    return static_cast<View *>(_window->tabs()->currentWidget());
}

}   // namespace Qelly

}   // namespace UJ
