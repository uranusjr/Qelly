/*****************************************************************************
 * main.cpp
 *
 * Created: 05/10 2011 by uranusjr
 *
 * Copyright 2013 uranusjr. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "Encodings.h"
#include "Controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Shortcuts are Cmd-based on Macs and Alt-based on others for us!!
    // DO NOT use Qt::MetaModifier, Qt::AltModifier, Qt::Key_Control and
    // Qt::Key_Meta, but use UJ::ModModifier, UJ::OptModifier, UJ::Key_Control
    // and UJ::Key_Meta in Globals.h instead.
    // As stated in Qt's documentation, Qt::META, Qt::CTRL, Qt::ALT and other
    // all-CAPS enums should be avoided, too.
    a.setAttribute(Qt::AA_MacDontSwapCtrlAndMeta);

    YL::init_table();
    QTranslator trans;
    trans.load(QLocale::system(), "Qelly", "_");
    a.installTranslator(&trans);

    UJ::Qelly::Controller c;
    Q_UNUSED(c);

    return a.exec();
}
