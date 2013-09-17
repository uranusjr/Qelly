#include <QApplication>
#include <QTranslator>
#include "Encodings.h"
#include "Controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Shortcuts are Cmd-based on Macs and Alt-based on others for us!!
    // DO NOT use Qt::META, Qt::ALT, Qt::Key_Control and Qt::Key_Meta, but use
    // UJ::MOD, UJ::ALT, UJ::Key_Control and UJ::Key_Meta in Globals.h instead.
    a.setAttribute(Qt::AA_MacDontSwapCtrlAndMeta);

    YL::init_table();

    QTranslator trans;
    trans.load(":/translations/UJ_Translate");
    a.installTranslator(&trans);

    UJ::Qelly::Controller c;
    Q_UNUSED(c);

    return a.exec();
}
