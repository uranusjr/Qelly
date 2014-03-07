# src.pro
#
# Created: 04/10 2011 by uranusjr
#
# Copyright 2011 uranusjr. All rights reserved.
#
# This file may be distributed under the terms of GNU Public License version
# 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
# license should have been included with this file, or the project in which
# this file belongs to. You may also find the details of GPL v3 at:
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# If you have any questions regarding the use of this file, feel free to
# contact the author of this file, or the owner of the project in which
# this file belongs to.


# Basic configs

QT       += core gui network

greaterThan(QT_VERSION, 4) {
    QT   += widgets
}

CONFIG   += qxt
QXT      += core gui widgets
TARGET   =  Qelly
TEMPLATE =  app
CONFIG   += precompile_header


# Extra compile flags

LIBS += -lsource-highlight

contains(DEFINES, QJSON) {
    LIBS += -lqjson
}

CONFIG(static) {
    win32-g++ {
        LIBS += -static-libgcc -static-libstdc++
    }
}


# Building configs

defineReplace(mode) {
    Debug:return(debug)
    Release:return(release)
}

BUILD_DIR = ../build/$$mode()
DESTDIR = ../bin/$$mode()
OBJECTS_DIR = $$BUILD_DIR
MOC_DIR = $$BUILD_DIR
RCC_DIR = $$BUILD_DIR
UI_DIR = $$BUILD_DIR
PRECOMPILED_DIR = $$BUILD_DIR


# Assets for deployment

TRANSLATIONS = \
    translations/Qelly.zh_TW.ts \
    translations/Qelly.zh_Hant.ts

include("translations.pri")
RC_FILE = icons/Qelly.rc        # Windows
ICON = icons/qower-icon.icns    # OS X


# Sources...

PRECOMPILED_HEADER = UJCommonDefs.h

SOURCES += main.cpp \
    MainWindow.cpp \
    SharedMenuBar.cpp \
    Terminal.cpp \
    Encodings.cpp \
    AbstractConnection.cpp \
    Site.cpp \
    Ssh.cpp \
    Telnet.cpp \
    TabWidget.cpp \
    View.cpp \
    UJQxWidget.cpp \
    Controller.cpp \
    SharedPreferences.cpp \
    PreferencesGeneral.cpp \
    PreferencesWindow.cpp \
    PreferencesFont.cpp \
    PreferencesColor.cpp \
    ColorWell.cpp \
    PreeditTextHolder.cpp \
    View_p.cpp \
    Tab.cpp \
    EmoticonViewer.cpp \
    JsonFileListModel.cpp \
    Globals.cpp \
    SiteManagerDialog.cpp \
    Json.cpp \
    CodePaster.cpp \
    PreferencesPaster.cpp

HEADERS  += \
    SharedMenuBar.h \
    MainWindow.h \
    Globals.h \
    YLTerminal.h \
    Terminal.h \
    Encodings.h \
    UJCommonDefs.h \
    AbstractConnection.h \
    Site.h \
    Ssh.h \
    Telnet.h \
    YLTelnet.h \
    TabWidget.h \
    View.h \
    UJQxWidget.h \
    Controller.h \
    SharedPreferences.h \
    PreferencesGeneral.h \
    PreferencesWindow.h \
    PreferencesFont.h \
    PreferencesColor.h \
    ColorWell.h \
    PreeditTextHolder.h \
    View_p.h \
    Tab.h \
    EmoticonViewer.h \
    JsonFileListModel.h \
    SiteManagerDialog.h \
    Json.h \
    CodePaster.h \
    PreferencesPaster.h

RESOURCES += \
    GeneralResources.qrc

FORMS += \
    PreferencesGeneral.ui \
    PreferencesFont.ui \
    PreferencesColor.ui \
    EmoticonViewer.ui \
    SiteManagerDialog.ui \
    CodePaster.ui \
    PreferencesPaster.ui

OTHER_FILES += \
    icons/Qelly.rc \
    about.html \
    acknowledgement.html
