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
#include "JsonFileListModel.h"
#include "Site.h"

namespace UJ
{

namespace Qelly
{

SiteManagerDialog::SiteManagerDialog(QWidget *parent) :
    QDialog(parent), _currentSite(0),
    _ui(new Ui::SiteManagerDialog)
{
    _ui->setupUi(this);
    _ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Connect");
    _ui->encodingComboBox->insertItem(BBS::EncodingBig5, tr("Big 5"));
    _ui->encodingComboBox->insertItem(BBS::EncodingGBK, tr("GBK"));
    _ui->ansiColorKeyComboBox->insertItem(BBS::ColorKeyCtrlU, tr("^U"));
    _ui->ansiColorKeyComboBox->insertItem(BBS::ColorKeyDoubleEsc,
                                          tr("ESC ESC"));

    connect(_ui->nameLineEdit, SIGNAL(textChanged(QString)),
            SLOT(setSiteName(QString)));
    connect(_ui->addressLineEdit, SIGNAL(textChanged(QString)),
            SLOT(setSiteFullForm(QString)));
    connect(_ui->encodingComboBox, SIGNAL(currentIndexChanged(int)),
            SLOT(setSiteEncoding(int)));
    connect(_ui->ansiColorKeyComboBox, SIGNAL(currentIndexChanged(int)),
            SLOT(setSiteColorKey(int)));
    connect(_ui->detectDoubleByteCheckBox, SIGNAL(toggled(bool)),
            SLOT(setSiteManualDoubleByte(bool)));
    connect(_ui->addButton, SIGNAL(clicked()), SLOT(addNewSite()));
    connect(_ui->removeButton, SIGNAL(clicked()), SLOT(removeCurrentSite()));
    connect(_ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(_ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(_ui->listView, SIGNAL(clicked(QModelIndex)),
            SLOT(displaySiteDetailAtIndex(QModelIndex)));

    QString filePath = absoluteDataStoragePath("sites.json");
    QAbstractItemModel *model = new JsonFileListModel(filePath);
    _ui->listView->setModel(model);
    _headers.clear();
    for (int i = 0; i < model->columnCount(); i++)
        _headers.insert(model->headerData(i, Qt::Horizontal).toString(), i);
}

SiteManagerDialog::~SiteManagerDialog()
{
    delete _ui;
    if (_currentSite)
        _currentSite->deleteLater();
}

void SiteManagerDialog::accept()
{
    // TODO: Implement connectWithSite in controller
    QString address = _currentSite->fullForm();
    if (!address.isEmpty())
        emit connectRequested(address);
    close();
}

void SiteManagerDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    QModelIndex index = _ui->listView->model()->index(0, 0);
    displaySiteDetailAtIndex(index);
}

void SiteManagerDialog::displaySiteDetailAtIndex(const QModelIndex &index)
{
    if (index == _ui->listView->currentIndex())
        return;
    if (_currentSite)
        delete _currentSite;

    _ui->listView->setCurrentIndex(index);
    int row = index.row();
    QAbstractItemModel *model = _ui->listView->model();
    QMap<QString, QVariant> properties;
    for (int c = 0; c < model->columnCount(); c++)
    {
        properties.insert(model->headerData(c, Qt::Horizontal).toString(),
                          model->data(model->index(row, c)));
    }
    _currentSite = Connection::Site::fromProperties(properties);
    _ui->nameLineEdit->setText(_currentSite->name());
    _ui->addressLineEdit->setText(_currentSite->fullForm());
    _ui->encodingComboBox->setCurrentIndex(_currentSite->encoding());
    _ui->ansiColorKeyComboBox->setCurrentIndex(_currentSite->colorKey());
    _ui->detectDoubleByteCheckBox->setCheckState(
                _currentSite->manualDoubleByte() ? Qt::Checked : Qt::Unchecked);
}

void SiteManagerDialog::setSiteName(const QString &name)
{
    setSiteProperty("name", name);
}

void SiteManagerDialog::setSiteFullForm(const QString &form)
{
    setSiteProperty("fullForm", form);
}

void SiteManagerDialog::setSiteEncoding(int encoding)
{
    setSiteProperty("encoding", encoding);
}

void SiteManagerDialog::setSiteColorKey(int k)
{
    setSiteProperty("colorKey", k);
}

void SiteManagerDialog::setSiteManualDoubleByte(bool db)
{
    setSiteProperty("doubleByte", db);
}

void SiteManagerDialog::addNewSite()
{
    if (_currentSite)
        delete _currentSite;

    _currentSite = new Connection::Site();
    QAbstractItemModel *model = _ui->listView->model();
    int row = model->rowCount();
    model->insertRow(row);

    for (QHash<QString, int>::const_iterator i = _headers.constBegin();
            i != _headers.constEnd(); i++)
    {
        QVariant v = _currentSite->property(i.key().toUtf8().data());
        model->setData(model->index(row, i.value()), v);
    }
    displaySiteDetailAtIndex(model->index(row, 0));
}

void SiteManagerDialog::removeCurrentSite()
{
    _ui->listView->model()->removeRow(_ui->listView->currentIndex().row());
}

void SiteManagerDialog::setSiteProperty(const char *name, QVariant v)
{
    if (!_currentSite)
        return;
    _currentSite->setProperty(name, v);

    QAbstractItemModel *model = _ui->listView->model();

    // Save the change back into the model
    Q_ASSERT(_headers.contains(name));
    int column = _headers.value(name);
    int row = _ui->listView->currentIndex().row();
    model->setData(model->index(row, column), v);
}

}   // namespace Qelly

}   // namespace UJ
