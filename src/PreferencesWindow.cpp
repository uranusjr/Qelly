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
#include "PreferencesColor.h"
#include "PreferencesFont.h"
#include "PreferencesGeneral.h"

namespace UJ
{

namespace Qelly
{

PreferencesWindow::PreferencesWindow(QWidget *parent) : QxtConfigDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    configWidget()->setIconSize(QSize(20, 20));

    QIcon general = QIcon(":/images/General.png");
    QIcon font = QIcon(":/images/Font.png");
    QIcon color = QIcon(":/images/Color.png");

    addPage(new PreferencesGeneral(this), general, tr("General"));
    addPage(new PreferencesFont(this), font, tr("Fonts"));
    addPage(new PreferencesColor(this), color, tr("Color"));

    setFixedWidth(550);
}

void PreferencesWindow::addPage(QWidget *page, QIcon &icon, QString title)
{
    configWidget()->addPage(page, icon, title);
}

}   // namespace Qelly

}   // namespace UJ
