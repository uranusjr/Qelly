/*****************************************************************************
 * JsonFileListModel.cpp
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

#include "JsonFileListModel.h"
#include <QFile>
#include <QxtJSON>

namespace UJ
{

class JsonFileListModelPrivate
{
    Q_DECLARE_PUBLIC(JsonFileListModel)
    JsonFileListModel *q_ptr;

public:
    JsonFileListModelPrivate(const QString &filename, JsonFileListModel *q) :
        q_ptr(q), file(new QFile(filename))
    {
        load();
    }
    ~JsonFileListModelPrivate()
    {
        save();
        file->deleteLater();
    }
    bool save()
    {
        if (file->open(QIODevice::WriteOnly | flags()))
        {
            cleanList();
            file->write(QxtJSON::stringify(list).toUtf8());
            file->close();
            return true;
        }
        return false;
    }
    void load()
    {
        if (file->open(QIODevice::ReadOnly | flags()))
        {
            list = QxtJSON::parse(file->readAll()).toList();
            file->close();
        }
    }
    void cleanList()
    {
        list.removeAll(QVariant());
        list.removeAll(QVariant(""));
    }

    QFile *file;
    QVariantList list;

private:
    inline static const QIODevice::OpenMode flags()
    {
        return QIODevice::Truncate | QIODevice::Text;
    }
};

JsonFileListModel::JsonFileListModel(const QString &filename, QObject *parent) :
    QAbstractListModel(parent),
    d_ptr(new JsonFileListModelPrivate(filename, this))
{
}

JsonFileListModel::~JsonFileListModel()
{
    delete d_ptr;
}

int JsonFileListModel::rowCount(const QModelIndex &) const
{
    return d_ptr->list.size();
}

QVariant JsonFileListModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return d_ptr->list.at(index.row());
    default:
        return QVariant();
    }
}

Qt::ItemFlags JsonFileListModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool JsonFileListModel::setData(
        const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
    {
        d_ptr->list[index.row()].setValue(value.toString());
        if (submit())
            emit dataChanged(index, index);
        return true;
    }
    default:
        return false;
    }
}

bool JsonFileListModel::insertRows(
        int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; i++)
        d_ptr->list.insert(i, "^.<");
    endInsertRows();
    return submit();
}

bool JsonFileListModel::removeRows(
        int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    while (count)
    {
        d_ptr->list.removeAt(row);
        count--;
    }
    endRemoveRows();
    return submit();
}

bool JsonFileListModel::submit()
{
    return d_ptr->save();
}

}   // namespace UJ
