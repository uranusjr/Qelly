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
#include <QLineEdit>
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
    _window->show();
    _window->setFixedSize(_window->fixedSize());
}

Controller::~Controller()
{
    _window->deleteLater();
}

void Controller::connectWithAddress(QString address)
{
    int current = _window->tabs()->currentIndex();
    // NOTE: Search saved sites for matching address
    QString name = address;
    _window->tabs()->setTabText(current, name);
    View *view = static_cast<View *>(_window->tabs()->widget(current));
    Connection::Terminal *t = new Connection::Terminal(view);
    Connection::AbstractConnection *connection;
    if (address.startsWith("ssh://"))
        return; // NOTE: Handle ssh
    else if (address.startsWith("telnet://"))
    {
        address = address.section("://", 1);
        connection = new Connection::Telnet();
        t->setConnection(connection, BBS::ConnectionTelnet);
    }
    else
    {
        connection = new Connection::Telnet();
        t->setConnection(connection, BBS::ConnectionTelnet);
    }
    view->setTerminal(t);
    view->setFocus(Qt::OtherFocusReason);
    connection->connectTo(address, 23);
}

void Controller::addTab()
{
    _window->tabs()->addTab(new View(), "");
    _window->address()->setFocus(Qt::ShortcutFocusReason);
}

void Controller::focusAddressField()
{
    _window->address()->setFocus(Qt::ShortcutFocusReason);
}

void Controller::closeTab()
{
    TabWidget *tabs = _window->tabs();
    tabs->closeTab(tabs->currentIndex());
}

void Controller::onAddressReturnPressed()
{
    if (!_window->tabs()->count())
        _window->tabs()->addTab(new View(), "");
    connectWithAddress(_window->address()->text());
}

}   // namespace Qelly

}   // namespace UJ
