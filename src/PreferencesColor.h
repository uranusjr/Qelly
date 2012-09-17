/*****************************************************************************
 * PreferencesColor.h
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

#ifndef PREFERENCESCOLOR_H
#define PREFERENCESCOLOR_H

#include <QWidget>
#include <QList>

namespace Ui
{
class PreferencesColor;
}

namespace UJ
{

class ColorWell;

namespace Qelly
{

class PreferencesColor : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesColor(QWidget *parent = 0);
    ~PreferencesColor();

public slots:
    void initialize();
    void cleanup();
    void accept();
    void reject();

private:
    Ui::PreferencesColor *_ui;
    QList<ColorWell *> normals;
    QList<ColorWell *> bolds;
};

}   // namespace Qelly

}   // namespace UJ

#endif // PREFERENCESCOLOR_H
