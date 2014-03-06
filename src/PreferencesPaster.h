#ifndef PREFERENCESPASTER_H
#define PREFERENCESPASTER_H

#include <QWidget>

namespace Ui {
class PreferencesPaster;
}

class PreferencesPaster : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesPaster(QWidget *parent = 0);
    ~PreferencesPaster();

private:
    Ui::PreferencesPaster *ui;
};

#endif // PREFERENCESPASTER_H
