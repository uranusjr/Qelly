#ifndef GUITESTER_H
#define GUITESTER_H

#include <QObject>
#include "MainWindow.h"

class GuiTester : public QObject
{
    Q_OBJECT
public:
    explicit GuiTester(QObject *parent = 0);

signals:

public slots:

private:
    UJ::Qelly::MainWindow *m;

};

#endif // GUITESTER_H
