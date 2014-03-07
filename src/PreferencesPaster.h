/*****************************************************************************
 * PreferencesPaster.h
 *
 * Created: 06/3 2014 by uranusjr
 *
 * Copyright 2014 uranusjr. All rights reserved.
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

#ifndef PREFERENCESPASTER_H
#define PREFERENCESPASTER_H

#include <QWidget>
class QStringList;
class QStandardItem;

namespace Ui
{
class PreferencesPaster;
}

namespace UJ
{

namespace Qelly
{

class PreferencesPaster : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesPaster(QWidget *parent = 0);
    ~PreferencesPaster();

public slots:
    void initialize();
    void cleanup();
    void accept();
    void reject();

private:
    Ui::PreferencesPaster *_ui;
    QStringList _langs;
    bool _visited;
};

}   // namespace Qelly

}   // namespace UJ

#endif // PREFERENCESPASTER_H
