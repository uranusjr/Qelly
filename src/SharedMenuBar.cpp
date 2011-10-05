#include "SharedMenuBar.h"
#include <QtGui/QMenu>

namespace UJ
{

SharedMenuBar::SharedMenuBar(QWidget *parent) : QMenuBar(parent)
{
    QMenu *file = new QMenu(tr("File"), this);
    addMenu(file);
}

}   // namespace UJ
