/*****************************************************************************
 * CodePaster.cpp
 *
 * Created: 03/5 2014 by uranusjr
 *
 * Copyright 2014 uranusjr. All rights reserved.
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

#include "CodePaster.h"
#include "ui_CodePaster.h"
#include <string>
#include <sstream>
#include <srchilite/sourcehighlight.h>

namespace UJ
{

namespace Qelly
{

CodePaster::CodePaster(QWidget *parent) :
    QDialog(parent), _ui(new Ui::CodePaster)
{
    _ui->setupUi(this);
    connect(_ui->buttons, SIGNAL(accepted()), SLOT(accept()));
    connect(_ui->buttons, SIGNAL(rejected()), SLOT(close()));
}

CodePaster::~CodePaster()
{
    delete _ui;
}

void CodePaster::accept()
{
    QString inputCode = _ui->codeEdit->toPlainText();

    std::stringstream input, output;
    input << inputCode.toUtf8().data();

    srchilite::SourceHighlight sourceHighlight("esc.outlang");
    // TODO: Provide a way to customize this
    sourceHighlight.highlight(input, output, "cpp.lang");

    std::string outputStr = output.str();
    QString highlighted =
        QString::fromUtf8(outputStr.c_str(), outputStr.length());

    // TODO: Do this only when the current connection is using ^U
    highlighted.replace('\x1b', '\x15');

    emit hasTextToInsert(highlighted);
    close();
}

}   // namespace Qelly

}   // namespace UJ
