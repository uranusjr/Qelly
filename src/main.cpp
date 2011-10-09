#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include "Encodings.h"
#include "Controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Shortcuts are Cmd-based on Macs and Alt-based on others for us!!
    // DO NOT use Qt::META and Qt::ALT, but use UJ::MOD and UJ::ALT in
    // Globals.h instead.
    a.setAttribute(Qt::AA_MacDontSwapCtrlAndMeta);

    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);

    YL::init_table();

    QTranslator trans;
    trans.load(":/translations/UJ_Translate");
    a.installTranslator(&trans);

    UJ::Qelly::Controller c;
    Q_UNUSED(c);

    return a.exec();
}
