/*****************************************************************************
 * PreferencesPaster.cpp
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

#include "PreferencesPaster.h"
#include "ui_PreferencesPaster.h"
#include <QSet>
#include <QStandardItemModel>
#include <QStringList>
#include <srchilite/langmap.h>
#include <srchilite/settings.h>
#include "Globals.h"
#include "SharedPreferences.h"

namespace UJ
{

namespace Qelly
{

PreferencesPaster::PreferencesPaster(QWidget *parent) :
    QWidget(parent), _ui(new Ui::PreferencesPaster)
{
    _ui->setupUi(this);

    // Read in the languages.
    srchilite::Settings settings;
    srchilite::LangMap map(settings.getDataDir(), "lang.map");
    map.open();

    // Build a non-repetitive list of languages.
    typedef srchilite::LangMap::const_iterator Iter;
    QSet<QString> langSet;
    for (Iter i = map.begin(); i != map.end(); i++)
    {
        QString lang = QString::fromUtf8(i->second.c_str(), i->second.length());
        langSet.insert(lang);
    }
    _langs = langSet.toList();
    qSort(_langs);
}

PreferencesPaster::~PreferencesPaster()
{
    delete _ui;
}

void PreferencesPaster::initialize()
{
    SharedPreferences *prefs = SharedPreferences::sharedInstance();
    QStringList activeLangs = prefs->activeLanguages();
    QStandardItemModel *model = new QStandardItemModel();
    foreach (const QString &lang, _langs)
    {
        QStandardItem *item = new QStandardItem(lang);
        item->setCheckable(true);
        if (activeLangs.contains(lang))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        model->appendRow(item);
    }

    _ui->languageList->setModel(model);
    _visited = true;
}

void PreferencesPaster::cleanup()
{
}

void PreferencesPaster::accept()
{
    if (!_visited)
        return;

    QStandardItemModel *model =
            qobject_cast<QStandardItemModel *>(_ui->languageList->model());
    QStringList activeLanguages;
    for (int row = 0; row < model->rowCount(); row++)
    {
        QStandardItem *item = model->item(row);
        if (item->checkState() == Qt::Checked)
            activeLanguages << item->text();
    }
    SharedPreferences *prefs = SharedPreferences::sharedInstance();
    prefs->setActiveLanguages(activeLanguages);

    _visited = false;
}

void PreferencesPaster::reject()
{
    initialize();
    _visited = false;
}

}   // namespace Qelly

}   // namespace UJ
