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
#include <srchilite/parserexception.h>
#include <srchilite/sourcehighlight.h>
#include "SharedPreferences.h"

namespace UJ
{

namespace Qelly
{

static const ssize_t COMMON_LANGUAGES_SIZE = 12;

static const char * COMMON_LANGUAGES[COMMON_LANGUAGES_SIZE][2] = {
    {"C", "c.lang"},
    {"C++", "cpp.lang"},
    {"C#", "csharp.lang"},
    {"CSS", "css.lang"},
    {"HTML", "html.lang"},
    {"Java", "java.lang"},
    {"JavaScript", "javascript.lang"},
    {"LaTeX", "latex.lang"},
    {"Python", "python.lang"},
    {"Ruby", "ruby.lang"},
    {"Shell", "sh.lang"},
    {"XML", "xml.lang"}
};

CodePaster::CodePaster(QWidget *parent) :
    QDialog(parent), _ui(new Ui::CodePaster), _colorKey(BBS::ColorKeyCtrlU)
{
    _ui->setupUi(this);

    QFont font = SharedPreferences::sharedInstance()->defaultFont();
    font.setPointSize(QApplication::font().pointSize());
    _ui->codeEdit->setFont(font);

    for (ssize_t i = 0; i < COMMON_LANGUAGES_SIZE; i++)
    {
        _ui->syntaxSelect->addItem(
            QString::fromUtf8(COMMON_LANGUAGES[i][0]),
            QString::fromUtf8(COMMON_LANGUAGES[i][1]));
    }

    connect(_ui->buttons, SIGNAL(accepted()), SLOT(accept()));
    connect(_ui->buttons, SIGNAL(rejected()), SLOT(close()));
}

CodePaster::~CodePaster()
{
    delete _ui;
}

BBS::AnsiColorKey CodePaster::colorKey() const
{
    return _colorKey;
}

void CodePaster::setColorKey(BBS::AnsiColorKey key)
{
    _colorKey = key;
}

void CodePaster::accept()
{
    QString inputCode = _ui->codeEdit->toPlainText();

    std::stringstream input, output;
    input << inputCode.toUtf8().data();
    srchilite::SourceHighlight sourceHighlight("esc.outlang");

    QString langName = _ui->syntaxSelect->currentText();
    int index = _ui->syntaxSelect->findText(langName, Qt::MatchExactly);
    if (index != -1)                        // Is a selected value.
        langName = _ui->syntaxSelect->itemData(index).toString();
    else if (!langName.endsWith(".lang"))   // Try to correct custom input.
        langName.append(".lang");

    try
    {
        // Do highlighting.
        sourceHighlight.highlight(input, output, langName.toUtf8().data());
    }
    catch (srchilite::ParserException)
    {
        // Fail silently and output the raw source without modification.
        output.str(inputCode.toUtf8().data());
    }

    std::string outputStr = output.str();
    QString highlighted =
        QString::fromUtf8(outputStr.c_str(), outputStr.length());

    switch (colorKey())
    {
    case BBS::ColorKeyCtrlU:
        highlighted.replace('\x1b', '\x15');
        break;
    case BBS::ColorKeyDoubleEsc:
        highlighted.replace('\x1b', "\x1b\x1b");
        break;
    default:
        break;
    }

    emit hasTextToInsert(highlighted);
    close();
}

}   // namespace Qelly

}   // namespace UJ
