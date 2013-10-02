/*****************************************************************************
 * SiteManagerDialog.h
 *
 * Created: 02/10 2013 by uranusjr
 *
 * Copyright 2013 uranusjr. All rights reserved.
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

#ifndef SITEMANAGERDIALOG_H
#define SITEMANAGERDIALOG_H

#include <QDialog>

namespace Ui
{
class SiteManagerDialog;
}

namespace UJ
{

namespace Qelly
{

class SiteManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SiteManagerDialog(QWidget *parent = 0);
    ~SiteManagerDialog();

public slots:
    virtual void accept();

private:
    Ui::SiteManagerDialog *_ui;
};

}   // namespace Qelly

}   // namespace UJ

#endif // SITEMANAGERDIALOG_H
