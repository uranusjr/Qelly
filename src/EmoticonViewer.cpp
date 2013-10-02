/*****************************************************************************
 * EmoticonViewer.cpp
 *
 * Created: 2013/10/1 2013 by uranusjr
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

#include "EmoticonViewer.h"
#include "ui_EmoticonViewer.h"
#include <QKeyEvent>
#include "Globals.h"
#include "JsonFileListModel.h"

namespace UJ
{

namespace Qelly
{

EmoticonViewer::EmoticonViewer(QWidget *parent) :
    QDialog(parent), _ui(new Ui::EmoticonViewer)
{
    _ui->setupUi(this);
    connect(_ui->buttons, SIGNAL(accepted()), SLOT(accept()));
    connect(_ui->buttons, SIGNAL(rejected()), SLOT(close()));
    connect(_ui->addButton, SIGNAL(clicked()), SLOT(addEntry()));
    connect(_ui->listView, SIGNAL(doubleClicked(QModelIndex)), SLOT(accept()));
    _ui->listView->installEventFilter(this);
    QString filePath = absoluteDataStoragePath("emoticons.json");
    _ui->listView->setModel(new JsonFileListModel(filePath));
}

EmoticonViewer::~EmoticonViewer()
{
    delete _ui;
}

bool EmoticonViewer::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == _ui->listView)
    {
        switch (e->type())
        {
        case QEvent::KeyPress:
        {
            int key = static_cast<QKeyEvent *>(e)->key();
            if (key == Qt::Key_Delete || key == Qt::Key_Backspace)
                deleteSelected();
            break;
        }
        default:
            break;
        }
    }
    return false;
}

void EmoticonViewer::accept()
{
    QListView *view = _ui->listView;
    emit hasTextToInsert(view->model()->data(view->currentIndex()).toString());
    close();
}

void EmoticonViewer::addEntry()
{
    QAbstractItemModel *model = _ui->listView->model();
    int row = model->rowCount();
    model->insertRow(row);
    _ui->listView->edit(model->index(row, 0));
}

void EmoticonViewer::deleteSelected()
{
    QListView *view = _ui->listView;
    view->model()->removeRow(view->currentIndex().row());
}

}   // namespace Qelly

}   // namespace UJ
