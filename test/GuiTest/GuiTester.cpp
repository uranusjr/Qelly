#include "GuiTester.h"

GuiTester::GuiTester(QObject *parent) : QObject(parent)
{
    m = new UJ::Qelly::MainWindow();
    m->show();
}
