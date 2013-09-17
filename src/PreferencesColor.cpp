/*****************************************************************************
 * PreferencesColor.cpp
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

#include "PreferencesColor.h"
#include "ui_PreferencesColor.h"
#include "SharedPreferences.h"

namespace UJ
{

namespace Qelly
{

PreferencesColor::PreferencesColor(QWidget *parent) : QWidget(parent)
{
    _ui = new Ui::PreferencesColor;
    _ui->setupUi(this);

    _normals << _ui->colorBlack  << _ui->colorRed  << _ui->colorGreen
             << _ui->colorYellow << _ui->colorBlue << _ui->colorMagenta
             << _ui->colorCyan   << _ui->colorWhite;
    _bolds << _ui->colorBlackBold << _ui->colorRedBold
           << _ui->colorGreenBold << _ui->colorYellowBold
           << _ui->colorBlueBold  << _ui->colorMagentaBold
           << _ui->colorCyanBold  << _ui->colorWhiteBold;

    initialize();
}

PreferencesColor::~PreferencesColor()
{
    delete _ui;
}

void PreferencesColor::initialize()
{
    SharedPreferences *prefs = SharedPreferences::sharedInstance();

    _ui->colorBackground->setColor(prefs->backgroundColor());

    for (int i = 0; i < 8; i++)
    {
        _normals.at(i)->setColor(prefs->fColor(i, false));
        _bolds.at(i)->setColor(prefs->fColor(i, true));
    }
    _visited = true;
}

void PreferencesColor::cleanup()
{
}

void PreferencesColor::accept()
{
    if (!_visited)
        return;

    SharedPreferences *prefs = SharedPreferences::sharedInstance();

    QColor color = _ui->colorBackground->color();
    if (color != prefs->backgroundColor())
        prefs->setBackgroundColor(color);

    for (int i = 0; i < 8; i++)
    {
        color = _normals.at(i)->color();
        if (color != prefs->fColor(i, false))
            prefs->setColor(color, i, false);

        color = _bolds.at(i)->color();
        if (color != prefs->fColor(i, true))
            prefs->setColor(color, i, true);
    }

    _visited = false;
}

void PreferencesColor::reject()
{
    initialize();
    _visited = false;
}

}   // namespace Qelly

}   // namespace UJ
