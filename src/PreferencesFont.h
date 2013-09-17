/*****************************************************************************
 * PreferencesFont.h
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

#ifndef PREFERENCESFONT_H
#define PREFERENCESFONT_H

#include <QWidget>

namespace Ui
{
class PreferencesFont;
}

namespace UJ
{

namespace Qelly
{

class PreferencesFont : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesFont(QWidget *parent = 0);
    ~PreferencesFont();

public slots:
    void initialize();
    void cleanup();
    void accept();
    void reject();

signals:
    void preferencesUpdated();

private slots:
    void browseFontRoman();
    void browseFontDoubleByte();

private:
    void setFontFieldRoman(QFont &font);
    void setFontFieldDoubleByte(QFont &font);
    Ui::PreferencesFont *_ui;
    bool _visited;
};

}   // namespace Qelly

}   // namespace UJ

#endif // PREFERENCESFONT_H
