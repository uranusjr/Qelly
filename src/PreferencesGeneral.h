/*****************************************************************************
 * PreferencesGeneral.h
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

#ifndef PREFERENCESGENERAL_H
#define PREFERENCESGENERAL_H

#include <QWidget>

namespace Ui
{
class PreferencesGeneral;
}

namespace UJ
{

namespace Qelly
{

class PreferencesGeneral : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesGeneral(QWidget *parent = 0);
    ~PreferencesGeneral();

public slots:
    void initialize();
    void cleanup();
    void accept();
    void reject();

private slots:
    void browseSshClientPath();

private:
    Ui::PreferencesGeneral *_ui;
};

}   // namespace Qelly

}   // namespace UJ

#endif // PREFERENCESGENERAL_H
