/*****************************************************************************
 * Globals.cpp
 *
 * Created: 2013/10/2 2013 by uranusjr
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

#include "Globals.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QStandardPaths>
#else
    #include <QDesktopServices>
#endif

namespace UJ
{

inline static const QDir &applicationPath()
{
    static QDir path(QCoreApplication::applicationDirPath());
    return path;
}

QString absoluteDataStoragePath(const QString &filename)
{
    if (isPortableMode())
        return applicationPath().filePath(filename);

    QString dirPath =
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
            QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
    QDir dir(dirPath);
    if (!dir.exists())
        dir.mkpath(".");
    return dir.filePath(filename);
}

QByteArray fromFile(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
        return QByteArray();
    QByteArray bytes = f.readAll();
    f.close();
    return bytes;
}

bool isPortableMode()
{
    // Making portableMode static means that the PORTABLE file check occurs only
    // once, when this function is being accessed the first time. This avoids
    // tricky situations in which the user might create/delete the check-file
    // while the program is running, since the mode will not change unless the
    // user restarts the program.
    static bool portableMode = applicationPath().exists("PORTABLE");
    return portableMode;
}

}   // namespace UJ
