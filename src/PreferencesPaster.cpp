#include "PreferencesPaster.h"
#include "ui_PreferencesPaster.h"

PreferencesPaster::PreferencesPaster(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreferencesPaster)
{
    ui->setupUi(this);
}

PreferencesPaster::~PreferencesPaster()
{
    delete ui;
}
