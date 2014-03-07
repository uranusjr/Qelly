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
#include <QDialogButtonBox>
#include <QIcon>
#include <QMetaObject>
#include <QPushButton>
#include <QTableWidget>
#include <QxtSignalGroup>
#include "PreferencesColor.h"
#include "PreferencesFont.h"
#include "PreferencesGeneral.h"
#include "PreferencesPaster.h"
#include "SharedPreferences.h"

namespace UJ
{

namespace Qelly
{

PreferencesWindow::PreferencesWindow(QWidget *parent) :
    QxtConfigDialog(parent), _displayPreferenceGroup(new QxtSignalGroup(this))
{
    setWindowTitle(tr("Preferences"));
    configWidget()->setIconSize(QSize(30, 30));
    connect(dialogButtonBox()->addButton(QDialogButtonBox::Apply),
            SIGNAL(clicked()), SLOT(apply()));

    PreferencesGeneral *general = new PreferencesGeneral(this);
    PreferencesFont *font = new PreferencesFont(this);
    PreferencesColor *color = new PreferencesColor(this);
    PreferencesPaster *paster = new PreferencesPaster(this);

    addPage(general, QIcon(":/images/preferences-system.png"),
            tr("General"));
    addPage(font, QIcon(":/images/preferences-desktop-font.png"),
            tr("Fonts"));
    addPage(color, QIcon(":/images/preferences-desktop-color.png"),
            tr("Color"));
    addPage(paster, QIcon(":/images/tools-wizard.png"), tr("Pasters"));

    // If anyone of the pages has been updated, trigger redraw
    _displayPreferenceGroup->addSignal(font, SIGNAL(preferencesUpdated()));
    _displayPreferenceGroup->addSignal(color, SIGNAL(preferencesUpdated()));
    connect(_displayPreferenceGroup, SIGNAL(firstSignalReceived()),
            SharedPreferences::sharedInstance(), SLOT(sync()));
    connect(_displayPreferenceGroup, SIGNAL(firstSignalReceived()),
            SIGNAL(displayPreferenceChanged()), Qt::QueuedConnection);
    connect(_displayPreferenceGroup, SIGNAL(firstSignalReceived()),
            _displayPreferenceGroup, SLOT(reset()));

    setFixedWidth(550);
}

void PreferencesWindow::addPage(QWidget *page, const QIcon &icon, QString title)
{
    configWidget()->addPage(page, icon, title);
}

void PreferencesWindow::apply()
{
    configWidget()->accept();
    _displayPreferenceGroup->reset();
}

}   // namespace Qelly

}   // namespace UJ
