/*****************************************************************************
 * TerminalTester.h
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

#ifndef TERMINALTESTER_H
#define TERMINALTESTER_H

#include <UJQxTestUtilities.h>
#include "../../src/Telnet.h"
#include "../../src/Terminal.h"

class TerminalTester : public UJ::Qx::Tester
{
    Q_OBJECT

public:

    explicit TerminalTester(QObject *parent = 0);
    UJ::Connection::Telnet connection;
    UJ::Connection::Terminal terminal;

public slots:
    void dumpProcessedData();
    void testConnect();
    void testLogin();
    void testInto();
};

#endif // TERMINALTESTER_H
