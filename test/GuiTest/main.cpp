#include <QApplication>
#include <QTextCodec>
#include "../../src/Encodings.h"
#include "GuiTester.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);

    GuiTester t;

    // See main.cpp in src/
    a.setAttribute(Qt::AA_MacDontSwapCtrlAndMeta);

    YL::init_table();

    return a.exec();
}
