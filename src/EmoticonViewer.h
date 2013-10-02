/*****************************************************************************
 * EmitocionViewer.h
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

#ifndef EMOTICONVIEWER_H
#define EMOTICONVIEWER_H

#include <QDialog>

namespace Ui
{
class EmoticonViewer;
}

namespace UJ
{

namespace Qelly
{

class EmoticonViewer : public QDialog
{
    Q_OBJECT

public:
    explicit EmoticonViewer(QWidget *parent = 0);
    ~EmoticonViewer();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);

signals:
    void hasTextToInsert(QString text);

private slots:
    void accept();
    void addEntry();
    void deleteSelected();

private:
    void loadEmoticons();
    void saveEmoticons();
    Ui::EmoticonViewer *_ui;
};

}   // namespace Qelly

}   // namespace UJ

#endif // EMOTICONVIEWER_H
