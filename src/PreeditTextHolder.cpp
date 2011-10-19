/*****************************************************************************
 * PreeditTextHolder.cpp
 *
 * Created: 19/10 2011 by uranusjr
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

#include "PreeditTextHolder.h"
#include "SharedPreferences.h"

namespace UJ
{

namespace Qelly
{

PreeditTextHolder::PreeditTextHolder(QWidget *parent) : QLineEdit(parent)
{
    _prefs = SharedPreferences::sharedInstance();
    _borderWidth = 2;
    _borderRadius = 8;
    _padding = 8;
    int height = _prefs->cellHeight() + _borderRadius;
    QString style;
    QTextStream stream(&style);
    stream << "UJ--Qelly--PreeditTextHolder {"
           << "color: white;"
           << "font:" << _prefs->doubleByteFont().pointSize() * 0.9 << "pt;"
           << "min-height:" << height << "px;"
           << "background-color: black;"
           << "border:" << _borderWidth << "px solid white;"
           << "border-radius:" << _borderRadius << "px;"
           << "}";
    setStyleSheet(style);
    setReadOnly(true);
    hide();
}

void PreeditTextHolder::updateText(const QString &text)
{
    setText(text);
    _textWidth =  QFontMetrics(font()).boundingRect(text).size().width();
    resize(_textWidth + _borderRadius * 2,  height());
}

}   // namespace Qelly

}   // namespace UJ
