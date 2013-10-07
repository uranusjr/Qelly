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
#include <QFileInfo>
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
            SLOT(browseSshClientPath()));
    connect(_ui->sshClientEnabled, SIGNAL(toggled(bool)),
            SLOT(setSshSettingsVisible(bool)));

    initialize();
}

PreferencesGeneral::~PreferencesGeneral()
{
    delete _ui;
}

void PreferencesGeneral::initialize()
{
    _visited = true;
    SharedPreferences *prefs = SharedPreferences::sharedInstance();\
    setSshSettingsVisible(prefs->isSshEnabled());
    _ui->sshClientEnabled->setChecked(prefs->isSshEnabled());
    _ui->sshClientPath->setText(prefs->sshClientPath());
    _ui->confirmCloseChecker->setChecked(prefs->warnOnClose());
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
    prefs->setWarnOnClose(_ui->confirmCloseChecker->isChecked());

    // Set up SSH configuration
    // If the SSH is set to enabled by the user, check whether a correct SSH
    // executable path is set. If the path provided is not valid, ignore the
    // path and disable SSH. If a valid path is provided, set the path in
    // prefs.
    bool isSshEnabled = _ui->sshClientEnabled->isChecked();
    if (isSshEnabled)
    {
        QString path = _ui->sshClientPath->text();
        if (QFileInfo(path).isExecutable())
        {
            prefs->setSshClientPath(path);
        }
        else
        {
            isSshEnabled = false;
            QString title = tr("SSH Executable Not Valid");
            QString msg = tr("To gain SSH functionality, you need to provide a "
                             "valid path to an external SSH client. SSH will "
                             "be disabled.");
            QMessageBox(QMessageBox::Warning, title, msg).exec();
        }
    }
    prefs->setSshEnabled(isSshEnabled);

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

void PreferencesGeneral::setSshSettingsVisible(bool value)
{
    _ui->sshClientPath->setVisible(value);
    _ui->sshClientPathBrowse->setVisible(value);
}

}   // namespace Qelly

}   // namespace UJ
