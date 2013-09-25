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
    buildToolBar();
    setUnifiedTitleAndToolBarOnMac(true);
    setMenuBar(SharedMenuBar::sharedInstance());
    _tabs = new TabWidget(this);
    _tabs->setTabPosition(QTabWidget::North);
    _prefs = SharedPreferences::sharedInstance();
    _firstRun = false;
    _contentHeight = 0;
    move(_prefs->windowPosition());
    setCentralWidget(_tabs);
    setWindowTitle("Qelly");
}

MainWindow::~MainWindow()
{
    delete _stretch;
    delete _inputFrame;
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

    QStyle *style = qApp->style();
    _toolbar = addToolBar(tr("General"));
    _toolbar->addAction(QIcon(":/images/Bookmarks.png"), tr("Sites"));
    _toolbar->addAction(QIcon(":/images/Reload.png"), tr("Reconnect"));
    _toolbar->addAction(QIcon(":/images/New.png"), tr("Add"));
    _toolbar->addWidget(_inputFrame);
    _toolbar->addWidget(_stretch);
    _toolbar->addAction(style->standardIcon(QStyle::SP_DirIcon),
                        tr("Emicons"));
    _toolbar->addAction(QIcon(":/images/Anti-sleep.png"), tr("Anti-Idle"));
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

void MainWindow::moveEvent(QMoveEvent *e)
{
    _prefs->setWindowPosition(e->pos());
    return QMainWindow::moveEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QSize size = e->size();
    int height = size.height();
    if (!_firstRun && height > _contentHeight)
    {
        _firstRun = true;
        setFixedSize(size.width(), height + 3);
    }

    QMainWindow::resizeEvent(e);
}

}   // namespace Qelly

}   // namespace UJ
