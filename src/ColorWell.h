/*****************************************************************************
 * ColorWell.h
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

#ifndef COLORWELL_H
#define COLORWELL_H

#include <QFrame>
#include <QColor>

namespace UJ
{

class ColorWell : public QFrame
{
    Q_OBJECT

public:
    explicit ColorWell(QWidget *parent = 0);
    inline QColor color() const { return _color; }
    void setColor(QColor c);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    QWidget *_colorMark;
    QColor _color;
};

}   // namespace UJ

#endif // COLORWELL_H
