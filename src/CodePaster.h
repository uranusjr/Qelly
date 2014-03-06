/*****************************************************************************
 * CodePaster.h
 *
 * Created: 05/3 2014 by uranusjr
 *
 * Copyright 2014 uranusjr. All rights reserved.
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

#ifndef CODEPASTER_H
#define CODEPASTER_H

#include <QDialog>
#include "Globals.h"

namespace Ui
{
class CodePaster;
}

namespace UJ
{

namespace Qelly
{

class CodePaster : public QDialog
{
    Q_OBJECT

public:
    explicit CodePaster(QWidget *parent = 0);
    ~CodePaster();

    BBS::AnsiColorKey colorKey() const;
    void setColorKey(BBS::AnsiColorKey key);

signals:
    void hasTextToInsert(const QString &text);

private slots:
    void accept();

private:
    Ui::CodePaster *_ui;
    BBS::AnsiColorKey _colorKey;
};

}   // namespace Qelly

}   // namespace UJ

#endif // CODEPASTER_H
