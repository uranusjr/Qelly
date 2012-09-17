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
#include "PreferencesGeneral.h"
#include "PreferencesFont.h"

namespace UJ
{

namespace Qelly
{

PreferencesWindow::PreferencesWindow(QWidget *parent) : QxtConfigDialog(parent)
{
    QIcon general = QIcon(":/images/General.png");
    QIcon font = QIcon(":/images/Font.png");

    addPage(new PreferencesGeneral(this), general, tr("General"));
    addPage(new PreferencesFont(this), font, tr("Fonts"));
}

void PreferencesWindow::addPage(QWidget *page, QIcon &icon, QString title)
{
    configWidget()->addPage(page, icon, title);
}

}   // namespace Qelly

}   // namespace UJ
