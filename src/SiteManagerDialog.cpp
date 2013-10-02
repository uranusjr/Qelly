/*****************************************************************************
 * SiteManagerDialog.cpp
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

#include "SiteManagerDialog.h"
#include "ui_SiteManagerDialog.h"

namespace UJ
{

namespace Qelly
{

SiteManagerDialog::SiteManagerDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::SiteManagerDialog)
{
    _ui->setupUi(this);
    _ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Connect");
    connect(_ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(_ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
}

SiteManagerDialog::~SiteManagerDialog()
{
    delete _ui;
}

void SiteManagerDialog::accept()
{
    // TODO: Connect to selected site
    close();
}

}   // namespace Qelly

}   // namespace UJ
