/*****************************************************************************
 * TerminalTester.cpp
 *
 * Created: 08/10 2011 by uranusjr
 *
 * Copyright 2011 uranusjr. All rights reserved.
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

#include "TerminalTester.h"
#include <QTextCodec>
#include <QTimer>

TerminalTester::TerminalTester(QObject *parent) : Tester(parent)
{
    terminal.setConnection(&connection);
    connect(&terminal, SIGNAL(dataProcessed()),
            this, SLOT(dumpProcessedData()));
    connect(&connection, SIGNAL(connected()), this, SLOT(testLogin()));
    QTimer::singleShot(1000, this, SLOT(testConnect()));
}

void TerminalTester::dumpProcessedData()
{
    for (int y = 0; y < 24; y++)
    {
        UJ::BBS::Cell *cells = terminal.cellsAtRow(y);
        QByteArray row;
        for (int x = 0; x < 80; x++)
            row.append(cells[x].byte);
        QTextCodec *codec = QTextCodec::codecForName("Big5");
        QString string = codec->toUnicode(row);
        *_cout << string << endl;
    }
}

void TerminalTester::testConnect()
{
    QString host = QString("ptt.cc");
    connection.connectTo(host);
}

void TerminalTester::testLogin()
{
    QString name = "";
    connection.sendBytes(name.toAscii());
    connection.sendBytes(QByteArray(1, '\r'));
    QString password = "";
    connection.sendBytes(password.toAscii());
    connection.sendBytes(QByteArray(1, '\r'));
    QTimer::singleShot(3000, this, SLOT(testInto()));
}

void TerminalTester::testInto()
{
    connection.sendBytes(QByteArray(1, 'n'));
    connection.sendBytes(QByteArray(1, '\r'));
    connection.sendBytes(QByteArray(1, ' '));
}
