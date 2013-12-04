/*****************************************************************************
 * Json.cpp
 *
 * Created: 04/12 2013 by uranusjr
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

#include "Json.h"

#ifdef QJSON

#include <qjson/parser.h>
#include <qjson/serializer.h>

QVariant UJ::Json::load(const QByteArray &bytes)
{
    static QJson::Parser parser;
    bool ok = false;
    QVariant result = parser.parse(bytes, &ok);
    if (ok)
        return result;
    qWarning() << "Parser error on line" << parser.errorLine()
               << "\n" << parser.errorString();
    return ok ? result : QVariant();
}

QByteArray UJ::Json::dump(const QVariant &v)
{
    static QJson::Serializer serializer;
    return serializer.serialize(v);
}

#else

#include <QxtJSON>

QVariant UJ::Json::load(const QByteArray &bytes)
{
    return QxtJSON::parse(QString::fromUtf8(bytes.data(), bytes.size()));
}

QByteArray UJ::Json::dump(const QVariant &v)
{
    return QxtJSON::stringify(v).toUtf8();
}

#endif
