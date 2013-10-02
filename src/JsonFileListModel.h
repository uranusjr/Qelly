/*****************************************************************************
 * JsonFileListModel.h
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

#ifndef JSONFILELISTMODEL_H
#define JSONFILELISTMODEL_H

#include <QAbstractListModel>

namespace UJ
{

class JsonFileListModelPrivate;

class JsonFileListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(JsonFileListModel)

public:
    explicit JsonFileListModel(
            const QString &filename, const QString &columsKey = "labels",
            const QString &itemsKey = "items", QObject *parent = 0);
    virtual ~JsonFileListModel();
    virtual int rowCount(const QModelIndex & = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & = QModelIndex()) const;
    virtual QVariant data(
            const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(
            const QModelIndex &index, const QVariant &value, int role);
    virtual bool insertRows(
            int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(
            int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool submit();

private:
    JsonFileListModelPrivate *d_ptr;
};

}   // namespace UJ

#endif // JSONFILELISTMODEL_H
