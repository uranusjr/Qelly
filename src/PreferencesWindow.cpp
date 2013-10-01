/*****************************************************************************
 * PreferencesWindow.cpp
 *
 * Created: 17/9 2012 by uranusjr
 *
 * Copyright 2012 uranusjr. All rights reserved.
 *
 * This file is published under the Creative Commons 3.0.
 * http://creativecommons.org/licenses/by/3.0/
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/

#include "PreferencesWindow.h"
#include <QIcon>
#include <QMetaObject>
#include <QTableWidget>
#include <QxtSignalGroup>
#include "PreferencesColor.h"
#include "PreferencesFont.h"
#include "PreferencesGeneral.h"
#include "SharedPreferences.h"

namespace UJ
{

namespace Qelly
{

PreferencesWindow::PreferencesWindow(QWidget *parent) : QxtConfigDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    configWidget()->setIconSize(QSize(20, 20));
    QxtSignalGroup *displayPreferenceGroup = new QxtSignalGroup(this);

    PreferencesGeneral *general = new PreferencesGeneral(this);
    PreferencesFont *font = new PreferencesFont(this);
    PreferencesColor *color = new PreferencesColor(this);

    addPage(general, QIcon(":/images/General.png"), tr("General"));
    addPage(font, QIcon(":/images/Font.png"), tr("Fonts"));
    addPage(color, QIcon(":/images/Color.png"), tr("Color"));

    // If anyone of the pages has been updated, trigger redraw
    displayPreferenceGroup->addSignal(font, SIGNAL(preferencesUpdated()));
    displayPreferenceGroup->addSignal(color, SIGNAL(preferencesUpdated()));
    connect(displayPreferenceGroup, SIGNAL(firstSignalReceived()),
            SharedPreferences::sharedInstance(), SLOT(sync()));
    connect(displayPreferenceGroup, SIGNAL(firstSignalReceived()),
            SIGNAL(displayPreferenceChanged()), Qt::QueuedConnection);
    connect(displayPreferenceGroup, SIGNAL(firstSignalReceived()),
            displayPreferenceGroup, SLOT(reset()));

    setFixedWidth(550);
}

void PreferencesWindow::addPage(QWidget *page, const QIcon &icon, QString title)
{
    configWidget()->addPage(page, icon, title);
}

}   // namespace Qelly

}   // namespace UJ
