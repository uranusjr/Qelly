#include "MainWindow.h"
#include <QtGui/QMenuBar>
#include <QtGui/QWidget>
#include "SharedMenuBar.h"

namespace UJ
{

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    buildCentralWidget();
    setMenuBar(SharedMenuBar::sharedInstance());
}

void MainWindow::buildCentralWidget()
{
    QWidget *w = new QWidget(this);
    setCentralWidget(w);
}

}   // namespace UJ
