/*****************************************************************************
 * SharedMenuBar.cpp
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

#include "SharedMenuBar.h"
#include <QKeySequence>
#include <QMenu>
#include "Globals.h"
#include "SharedPreferences.h"

namespace UJ
{

namespace Qelly
{

SharedMenuBar::SharedMenuBar(QWidget *parent) : QMenuBar(parent)
{
    QMenu *menu = 0;
    QAction *action;
    SharedPreferences *prefs = SharedPreferences::sharedInstance();
    typedef QKeySequence Seq;

    connect(this, SIGNAL(editPreferences()), this, SIGNAL(preferences()));
    connect(this, SIGNAL(windowAbout()), this, SIGNAL(about()));

    menu = addMenu(tr("File"));
    menu->addAction(tr("New Tab"), this, SIGNAL(fileNewTab()),
                    Seq(UJ::ModModifier | Qt::Key_T));
    menu->addAction(tr("Open Location..."), this, SIGNAL(fileOpenLocation()),
                    Seq(UJ::ModModifier | Qt::Key_L));
    menu->addAction(tr("Reconnect"), this, SIGNAL(fileReconnect()));
    menu->addSeparator();
    menu->addAction(tr("Close Tab"), this, SIGNAL(fileCloseTab()),
                    Seq(UJ::ModModifier | Qt::Key_W));
    menu->addSeparator();
    menu->addAction(tr("Quit"), this, SIGNAL(fileQuit()),
                    Seq(Qt::AltModifier | Qt::Key_F4));

    menu = addMenu(tr("Edit"));
    menu->addAction(tr("Copy"), this, SIGNAL(editCopy()),
                    Seq(UJ::ModModifier | Qt::Key_C));
    menu->addAction(tr("Paste"), this, SIGNAL(editPaste()),
                    Seq(UJ::ModModifier | Qt::Key_V));
    menu->addAction(tr("Paste Wrap"), this, SIGNAL(editPasteWrap()),
                    Seq(UJ::ModModifier | Qt::ShiftModifier | Qt::Key_V));
#ifdef Q_WS_MAC
    menu->addAction(tr("Paste Color"), this, SIGNAL(editPasteColor()),
                    Seq(UJ::ModModifier | UJ::OptModifier | Qt::Key_V));
#else
    menu->addAction(tr("Paste Color"), this, SIGNAL(editPasteColor()));
#endif
    menu->addAction(tr("Select All"), this, SIGNAL(editSelectAll()),
                    Seq(UJ::ModModifier | Qt::Key_A));
    menu->addSeparator();
    menu->addAction(tr("Emoticons..."), this, SIGNAL(editEmoticons()),
                    Seq(UJ::ModModifier | Qt::Key_E));
    menu->addSeparator();
#ifdef Q_WS_MAC
    menu->addAction(tr("Customize Toolbar..."),
                    this, SIGNAL(editCustomizeToolbar()));
#endif
    menu->addAction(tr("Preferences..."), this, SIGNAL(editPreferences()),
                    Seq(UJ::ModModifier | Qt::Key_Comma));

    menu = addMenu(tr("View"));
    action = menu->addAction(tr("Anti-Idle"));
    action->setCheckable(true);
    action->setChecked(prefs->isAntiIdleActive());
    action->connect(prefs, SIGNAL(antiIdleChanged(bool)),
                    SLOT(setChecked(bool)));
    connect(action, SIGNAL(triggered(bool)), SIGNAL(viewAntiIdle(bool)));
    menu->addAction(tr("Show Hidden Text"), this, SIGNAL(viewShowHiddenText()));

    menu = addMenu(tr("Sites"));
    menu->addAction(tr("Edit Sites..."), this, SIGNAL(sitesEditSites()),
                    Seq(UJ::ModModifier | Qt::Key_B));
    menu->addAction(tr("Add This Site"), this, SIGNAL(siteAddThisSite()),
                    Seq(UJ::ModModifier | Qt::Key_D));

    menu = addMenu(tr("Window"));
    menu->addAction(tr("Minimize"), this, SIGNAL(windowMinimize()),
                    Seq(UJ::ModModifier | Qt::Key_M));
    menu->addAction(tr("Select Next Tab"), this, SIGNAL(windowSelectNextTab()),
                    Seq(UJ::ModModifier | Qt::Key_Right));
    menu->addAction(tr("Select Previous Tab"),
                    this, SIGNAL(windowSelectPreviousTab()),
                    Seq(UJ::ModModifier | Qt::Key_Left));
#ifdef Q_WS_MAC
    menu->addAction(tr("Bring All to Front"),
                    this, SIGNAL(windowBringAllToFront()),
                    Seq(UJ::ModModifier | UJ::OptModifier | Qt::Key_Up));
#endif
    menu = addMenu(tr("Help"));
    menu->addAction(tr("About"), this, SIGNAL(about()));
    menu->addAction(tr("Visit Project Home..."),
                    this, SIGNAL(helpVisitProjectHome()));
}

}   // namespace Qelly

}   // namespace UJ
