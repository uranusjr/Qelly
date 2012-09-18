/*****************************************************************************
 * PreferencesFont.cpp
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

#include "PreferencesFont.h"
#include "ui_PreferencesFont.h"
#include <QFontDialog>
#include "SharedPreferences.h"

namespace UJ
{

namespace Qelly
{

PreferencesFont::PreferencesFont(QWidget *parent) : QWidget(parent)
{
    _ui = new Ui::PreferencesFont;
    _ui->setupUi(this);
    connect(_ui->fontRomanNameChoose, SIGNAL(clicked()),
            this, SLOT(browseFontRoman()));
    connect(_ui->fontDoubleByteNameChoose, SIGNAL(clicked()),
            this, SLOT(browseFontDoubleByte()));

    initialize();
}

PreferencesFont::~PreferencesFont()
{
    delete _ui;
}

void PreferencesFont::initialize()
{
    SharedPreferences *prefs = SharedPreferences::sharedInstance();
    _ui->cellWidth->setValue(prefs->cellWidth());
    _ui->cellHeight->setValue(prefs->cellHeight());
    _ui->fontRomanMarginLeft->setValue(prefs->defaultFontPaddingLeft());
    _ui->fontRomanMarginBottom->setValue(prefs->defaultFontPaddingBottom());
    _ui->fontDoubleByteMarginLeft->setValue(prefs->doubleByteFontPaddingLeft());
    _ui->fontDoubleByteMarginBottom->setValue(
                                        prefs->doubleByteFontPaddingBottom());
    QFont font = prefs->defaultFont();
    setFontFieldRoman(font);
    font = prefs->doubleByteFont();
    setFontFieldDoubleByte(font);
    _visited = true;
}

void PreferencesFont::cleanup()
{
}

void PreferencesFont::accept()
{
    if (!_visited)
        return;

    SharedPreferences *prefs = SharedPreferences::sharedInstance();
    prefs->setCellWidth(_ui->cellWidth->value());
    prefs->setCellHeight(_ui->cellHeight->value());

    QFont font;
    if (font.fromString(_ui->fontRomanName->text()))
        prefs->setDefaultFont(font);
    if (font.fromString(_ui->fontDoubleByteName->text()))
        prefs->setDoubleByteFont(font);

    prefs->setDefaultFontPaddingLeft(_ui->fontRomanMarginLeft->value());
    prefs->setDefaultFontPaddingBottom(_ui->fontRomanMarginBottom->value());
    prefs->setDoubleByteFontPaddingLeft(_ui->fontDoubleByteMarginLeft->value());
    prefs->setDoubleByteFontPaddingBottom(
                                    _ui->fontDoubleByteMarginBottom->value());

    _visited = false;
}

void PreferencesFont::reject()
{
    initialize();
    _visited = false;
}

void PreferencesFont::browseFontRoman()
{
    SharedPreferences *prefs = SharedPreferences::sharedInstance();
    bool ok;
    QFont defaultFont = prefs->defaultFont();
    QFont font = QFontDialog::getFont(&ok, defaultFont, this);
    if (ok && (!font.isCopyOf(defaultFont)))
        setFontFieldRoman(font);
}

void PreferencesFont::browseFontDoubleByte()
{
    SharedPreferences *prefs = SharedPreferences::sharedInstance();
    bool ok;
    QFont defaultFont = prefs->doubleByteFont();
    QFont font = QFontDialog::getFont(&ok, defaultFont, this);
    if (ok && (!font.isCopyOf(defaultFont)))
        setFontFieldDoubleByte(font);
}

void PreferencesFont::setFontFieldRoman(QFont &font)
{
    QStringList attributes;
    attributes << font.family() << QString::number(font.pointSize());
    _ui->fontRomanName->setText(attributes.join(", "));
}

void PreferencesFont::setFontFieldDoubleByte(QFont &font)
{
    QStringList attributes;
    attributes << font.family() << QString::number(font.pointSize());
    _ui->fontDoubleByteName->setText(attributes.join(", "));
}

}   // namespace Qelly

}   // namespace UJ
