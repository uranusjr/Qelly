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
#include <QInputMethodEvent>
#include <QTextStream>
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
    const QFont &font = _prefs->doubleByteFont();

    QString style;
    QTextStream stream(&style);
    stream << "UJ--Qelly--PreeditTextHolder {" <<
              "color: #f0f0f0;" <<
              "font-family:" << font.family() << ";" <<
              "font-weight: 500;" <<    // Slightly bolder than normal (400)
              "font-size:" << font.pointSize() * 0.9 << "pt;" <<
              "background-color: black;" <<
              "border:" << _borderWidth << "px solid #f0f0f0;" <<
              "border-radius:" << _borderRadius << "px;" <<
              "}";
    setStyleSheet(style);
    resize(0, height());
}

int PreeditTextHolder::widthForText(const QString &text)
{
    int textWidth = QFontMetrics(font()).boundingRect(text).size().width();
    return textWidth + _borderRadius * 2;
}

void PreeditTextHolder::inputMethodEvent(QInputMethodEvent *e)
{
    QLineEdit::inputMethodEvent(e);
    if (e->preeditString().isEmpty())
        resize(0, height());
    else
        resize(widthForText(e->preeditString()), height());
    if (!e->commitString().isEmpty())
        emit hasCommitString(e);
}

}   // namespace Qelly

}   // namespace UJ
