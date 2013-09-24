/*****************************************************************************
 * PreferencesWindow.h
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

#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#include <QxtConfigDialog>

namespace UJ
{

namespace Qelly
{

class PreferencesWindow : public QxtConfigDialog
{
    Q_OBJECT

public:
    explicit PreferencesWindow(QWidget *parent = 0);
    void addPage(QWidget *page, const QIcon &icon, QString title);

signals:
    void displayPreferenceChanged();
};

}   // namespace Qelly

}   // namespace UJ

#endif // PREFERENCESWINDOW_H
