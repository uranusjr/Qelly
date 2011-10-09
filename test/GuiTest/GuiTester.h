#ifndef GUITESTER_H
#define GUITESTER_H

#include "../Test/UJQxTestUtilities.h"
#include "../../src/MainWindow.h"

class GuiTester : public UJ::Qx::Tester
{
    Q_OBJECT

public:
    explicit GuiTester(QObject *parent = 0);
    virtual ~GuiTester();

signals:

public slots:
    void newTab();

private:
    UJ::Qelly::MainWindow *m;

};

#endif // GUITESTER_H
