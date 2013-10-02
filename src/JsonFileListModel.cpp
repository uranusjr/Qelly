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
    JsonFileListModelPrivate(const QString &filename, const QString &cKey,
                             const QString &iKey, JsonFileListModel *q) :
        q_ptr(q), file(new QFile(filename)), labelsKey(cKey), itemsKey(iKey)
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
            file->write(QxtJSON::stringify(data).toUtf8());
            file->close();
            return true;
        }
        return false;
    }
    void load()
    {
        if (file->open(QIODevice::ReadOnly | flags()))
        {
            data = QxtJSON::parse(file->readAll()).toMap();
            file->close();
        }
    }

    QFile *file;
    QVariantMap data;
    QString labelsKey;
    QString itemsKey;

private:
    inline static const QIODevice::OpenMode flags()
    {
        return QIODevice::Truncate | QIODevice::Text;
    }
};

JsonFileListModel::JsonFileListModel(
        const QString &filename, const QString &columsKey,
        const QString &itemsKey, QObject *parent) :
    QAbstractListModel(parent),
    d_ptr(new JsonFileListModelPrivate(filename, columsKey, itemsKey, this))
{
}

JsonFileListModel::~JsonFileListModel()
{
    delete d_ptr;
}

int JsonFileListModel::rowCount(const QModelIndex &) const
{
    return d_ptr->data[d_ptr->itemsKey].toList().count();
}

int JsonFileListModel::columnCount(const QModelIndex &) const
{
    return d_ptr->data[d_ptr->labelsKey].toList().count();
}

QVariant JsonFileListModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
        QVariant v = d_ptr->data[d_ptr->itemsKey].toList().at(index.row());
        if (v.canConvert<QVariantList>())
            v = v.toList().at(index.column());
        return v;
    }
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
    Q_D(JsonFileListModel);
    switch (role)
    {
    case Qt::EditRole:
    {
        QVariantList items = d->data[d->itemsKey].toList();
        QVariant &item = items[index.row()];
        if (item.canConvert<QVariantList>())
        {
            QVariantList list = item.toList();
            list[index.column()].setValue(value.toString());
            item.setValue(list);
        }
        else
        {
            item.setValue(value.toString());
        }
        d->data[d->itemsKey].setValue(items);
        if (submit())
            emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    default:
        return false;
    }
}

bool JsonFileListModel::insertRows(
        int row, int count, const QModelIndex &parent)
{
    Q_D(JsonFileListModel);
    beginInsertRows(parent, row, row + count - 1);
    QVariantList items = d->data[d->itemsKey].toList();
    for (int i = row; i < row + count; i++)
    {
        QVariantList values;
        for (int j = 0; j < d->data[d->labelsKey].toList().count(); j++)
            values << QVariant("");
        items.insert(i, values.size() > 1 ? values : values.first());
    }
    d->data[d->itemsKey].setValue(items);
    endInsertRows();
    return submit();
}

bool JsonFileListModel::removeRows(
        int row, int count, const QModelIndex &parent)
{
    Q_D(JsonFileListModel);
    beginRemoveRows(parent, row, row + count - 1);
    QVariantList items = d->data[d->itemsKey].toList();
    while (count)
    {
        items.removeAt(row);
        count--;
    }
    d->data[d->itemsKey].setValue(items);
    endRemoveRows();
    return submit();
}

bool JsonFileListModel::submit()
{
    return d_ptr->save();
}

}   // namespace UJ
