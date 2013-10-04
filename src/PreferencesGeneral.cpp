/*****************************************************************************
 * PreferencesGeneral.cpp
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

#include "PreferencesGeneral.h"
#include "ui_PreferencesGeneral.h"
#include <QFileDialog>
#include <QMessageBox>
#include "SharedPreferences.h"

namespace UJ
{

namespace Qelly
{

PreferencesGeneral::PreferencesGeneral(QWidget *parent) : QWidget(parent)
{
    _ui = new Ui::PreferencesGeneral;
    _ui->setupUi(this);
    connect(_ui->sshClientPathBrowse, SIGNAL(clicked()),
            this, SLOT(browseSshClientPath()));

    initialize();
}

PreferencesGeneral::~PreferencesGeneral()
{
    delete _ui;
}

void PreferencesGeneral::initialize()
{
    _visited = true;
    SharedPreferences *prefs = SharedPreferences::sharedInstance();
    _ui->sshClientPath->setText(prefs->sshClientPath());
    _ui->restoreOnStartupChecker->setChecked(
                prefs->restoreConnectionsOnStartup());
}

void PreferencesGeneral::cleanup()
{
}

void PreferencesGeneral::accept()
{
    if (!_visited)
        return;

    SharedPreferences *prefs = SharedPreferences::sharedInstance();
    prefs->setRestoreConnectionsOnStartup(
                _ui->restoreOnStartupChecker->isChecked());
    try
    {
        prefs->setSshClientPath(_ui->sshClientPath->text());
    }
    catch (const char *errorMessage)
    {
        QString msg = tr("SSH will not be available until it is corrected.");
        QMessageBox(QMessageBox::Warning, errorMessage, msg).exec();
    }
    _visited = false;
}

void PreferencesGeneral::reject()
{
    initialize();
    _visited = false;
}

void PreferencesGeneral::browseSshClientPath()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choose SSH Client"),
                                                _ui->sshClientPath->text());
    if (!path.isNull())
        _ui->sshClientPath->setText(path);
}

}   // namespace Qelly

}   // namespace UJ
