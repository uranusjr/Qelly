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
#include "Json.h"

namespace UJ
{

class JsonFileListModelPrivate
{
    Q_DECLARE_PUBLIC(JsonFileListModel)
    JsonFileListModel *q_ptr;

public:
    JsonFileListModelPrivate(const QString &filename, const QStringList &ks,
                             JsonFileListModel *q) :
        q_ptr(q), file(new QFile(filename)), keys(ks)
    {
        if (keys.isEmpty())
            keys << "value";
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
            file->write(Json::dump(data));
            file->close();
            return true;
        }
        return false;
    }
    void load()
    {
        if (file->open(QIODevice::ReadOnly | flags()))
        {
            QByteArray bytes = file->readAll();
            data = Json::load(bytes.constData()).toList();
            file->close();
        }
    }

    QFile *file;
    QStringList keys;
    QVariantList data;

private:
    inline static const QIODevice::OpenMode flags()
    {
        return QIODevice::Truncate | QIODevice::Text;
    }
};

JsonFileListModel::JsonFileListModel(
        const QString &filename, const QStringList &keys, QObject *parent) :
    QAbstractListModel(parent),
    d_ptr(new JsonFileListModelPrivate(filename, keys, this))
{
}

JsonFileListModel::~JsonFileListModel()
{
    delete d_ptr;
}

int JsonFileListModel::rowCount(const QModelIndex &) const
{
    return d_ptr->data.size();
}

int JsonFileListModel::columnCount(const QModelIndex &) const
{
    return d_ptr->keys.size();
}

QVariant JsonFileListModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
        QVariant v;
        int row = index.row();
        if (!d_ptr->data.isEmpty() && row >= 0 && d_ptr->data.size() > row)
        {
            v = d_ptr->data.at(row);
            if (v.canConvert<QVariantList>())
            {
                QVariantList vl = v.toList();
                int column = index.column();
                if (!vl.isEmpty() && column >= 0 && vl.size() > column)
                    v = vl.at(index.column());
            }
        }
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
    switch (role)
    {
    case Qt::EditRole:
    {
        QVariant &item = d_ptr->data[index.row()];
        if (item.canConvert<QVariantList>())
        {
            QVariantList list = item.toList();
            list[index.column()].setValue(value);
            item.setValue(list);
        }
        else
        {
            item.setValue(value);
        }
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
    {
        QVariantList values;
        for (int j = 0; j < columnCount(); j++)
            values << QVariant();
        d_ptr->data.insert(i, values.size() > 1 ? values : values.first());
    }
    endInsertRows();
    return submit();
}

bool JsonFileListModel::removeRows(
        int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    while (count)
    {
        d_ptr->data.removeAt(row);
        count--;
    }
    endRemoveRows();
    return submit();
}

bool JsonFileListModel::submit()
{
    return d_ptr->save();
}

QVariant JsonFileListModel::headerData(
        int section, Qt::Orientation orientation, int role) const
{
    switch (orientation)
    {
    case Qt::Horizontal:
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            if (section < d_ptr->keys.size())
                return d_ptr->keys.at(section);
        default:
            return QVariant();
        }
    default:
        return QVariant();
    }
}

}   // namespace UJ
