/*****************************************************************************
 * PreeditTextHolder.h
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

#ifndef PREEDITTEXTHOLDER_H
#define PREEDITTEXTHOLDER_H

#include <QLineEdit>

namespace UJ
{

namespace Qelly
{

class SharedPreferences;

class PreeditTextHolder : public QLineEdit
{
    Q_OBJECT

public:
    explicit PreeditTextHolder(QWidget *parent = 0);
    int widthForText(const QString &text);

signals:
    void hasCommitString(QInputMethodEvent *);

protected:
    virtual void inputMethodEvent(QInputMethodEvent *e);

private:
    SharedPreferences *_prefs;
    int _borderWidth;
    int _borderRadius;
    int _padding;
    int _textWidth;

public: // Setters & Getters
    inline int textWidth() const
    {
        return _textWidth;
    }
    inline int margin() const
    {
        return _borderWidth + _borderRadius;
    }
};

}   // namespace Qelly

}   // namespace UJ

#endif // PREEDITTEXTHOLDER_H
