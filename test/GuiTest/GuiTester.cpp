#include "GuiTester.h"
#include "../../src/TabWidget.h"
#include "../../src/SharedMenuBar.h"
#include "../../src/View.h"

GuiTester::GuiTester(QObject *parent) : Tester(parent)
{
    m = new UJ::Qelly::MainWindow();
    connect(UJ::Qelly::SharedMenuBar::sharedInstance(),
            SIGNAL(fileNewTab()), this, SLOT(newTab()));
    m->show();
}

GuiTester::~GuiTester()
{
    m->deleteLater();
}

void GuiTester::newTab()
{
    QString t("批踢踢實業坊");
    m->tabs()->addTab(new UJ::Qelly::View(), t);
}
