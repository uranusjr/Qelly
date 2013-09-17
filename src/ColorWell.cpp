/*****************************************************************************
 * ColorWell.cpp
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

#include "ColorWell.h"
#include <QColorDialog>

namespace UJ
{

ColorWell::ColorWell(QWidget *parent) : QFrame(parent)
{
    _color = QColor(Qt::transparent);

    _colorMark = new QWidget(this);
    _colorMark->setFixedSize(28, 10);

    _colorMark->move(6, 5);
    setFixedSize(40, 20);

    QPalette p = palette();
    p.setColor(QPalette::Background, QColor(Qt::lightGray));
    setPalette(p);

    setLineWidth(1);
    setFrameStyle(QFrame::Box);
    setStyleSheet("QFrame{color:gray; background-color:light gray;}");
}

void ColorWell::mousePressEvent(QMouseEvent *)
{
    setStyleSheet("QFrame{color:gray; background-color:gray;}");

    QColor c = QColorDialog::getColor(_color, parentWidget());

    if (c.isValid() && c != color())    // c is invalid if getColor is cancelled
        setColor(c);

    setStyleSheet("QFrame{color:gray; background-color:light gray;}");
}

void ColorWell::mouseReleaseEvent(QMouseEvent *)
{
    setStyleSheet("QFrame{color:gray; background-color:light gray;}");
}

void ColorWell::setColor(QColor c)
{
    _color = c;
    _colorMark->setStyleSheet(QString("background-color:%1").arg(c.name()));
}

}   // namespace UJ
