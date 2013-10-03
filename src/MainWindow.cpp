/*****************************************************************************
 * MainWindow.cpp
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

#include "MainWindow.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QTabBar>
#include <QToolBar>
#include <QStyle>
#include "Globals.h"
#include "SharedMenuBar.h"
#include "SharedPreferences.h"
#include "TabWidget.h"
#include "View.h"

namespace UJ
{

namespace Qelly
{

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    _tabs = new TabWidget(this);
    _tabs->setTabPosition(QTabWidget::North);
    _prefs = SharedPreferences::sharedInstance();

    buildToolBar();
    setUnifiedTitleAndToolBarOnMac(true);
    setMenuBar(SharedMenuBar::sharedInstance());
    setCentralWidget(_tabs);
    setWindowTitle("Qelly");

    restoreGeometry(_prefs->windowGeometry());
}

void MainWindow::buildToolBar()
{
    _stretch = new QWidget();
    _stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    _address = new QLineEdit();
    _address->setPlaceholderText(tr("Input site address"));
    _address->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _address->setFocusPolicy(Qt::ClickFocus);
    QLabel *inputLabel = new QLabel(QString("<small>") +
                                    tr("Address") +
                                    QString("</small>"));
    inputLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    inputLabel->setAlignment(Qt::AlignCenter);
    inputLabel->setFocusPolicy(Qt::ClickFocus);
    _inputFrame = new QWidget();
    _inputFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QVBoxLayout *inputLayout = new QVBoxLayout(_inputFrame);
    inputLayout->addWidget(_address);
    inputLayout->addWidget(inputLabel);
    _inputFrame->setLayout(inputLayout);

    QAction *action;
    QStyle *style = qApp->style();
    _toolbar = addToolBar(tr("General"));
    _toolbar->addAction(QIcon(":/images/Bookmarks.png"), tr("Sites"),
                        this, SIGNAL(siteManageShouldOpen()));
    _toolbar->addAction(QIcon(":/images/Reload.png"), tr("Reconnect"),
                        this, SIGNAL(reconnect()));
    _toolbar->addAction(QIcon(":/images/New.png"), tr("Add"),
                        this, SIGNAL(newTabRequested()));
    _toolbar->addWidget(_inputFrame);
    _toolbar->addWidget(_stretch);
    _toolbar->addAction(
                style->standardIcon(QStyle::SP_DirIcon), tr("Emoticons"),
                this, SIGNAL(emoticonViewerShouldOpen()));
    action = _toolbar->addAction(QIcon(":/images/Anti-sleep.png"),
                                 tr("Anti-Idle"));
    action->setCheckable(true);
    action->setChecked(_prefs->isAntiIdleActive());
    connect(action, SIGNAL(triggered(bool)), SIGNAL(antiIdleTriggered(bool)));
    action->connect(_prefs, SIGNAL(antiIdleChanged(bool)),
                    SLOT(setChecked(bool)));
    _toolbar->addAction(QIcon(":/images/Flashlight.png"), tr("Peek"));
    _toolbar->addAction(style->standardIcon(QStyle::SP_DirIcon),
                        tr("Double Byte"));
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    e->ignore();
    emit windowShouldClose();
}

bool MainWindow::event(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::Resize:
    case QEvent::Move:
        _prefs->setWindowGeometry(saveGeometry());
        break;
    case QEvent::WindowStateChange:
    {
        // When return to normal state from maximized state, automatically
        // adapt the recommended size.
        QWindowStateChangeEvent *ce = static_cast<QWindowStateChangeEvent *>(e);
        if ((ce->oldState() & Qt::WindowMaximized) && !isMaximized())
            resize(sizeHint());
    }
    default:
        break;
    }
    return QMainWindow::event(e);
}

}   // namespace Qelly

}   // namespace UJ
