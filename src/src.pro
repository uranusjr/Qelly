#-------------------------------------------------
#
# Project created by QtCreator 2011-10-04T01:19:38
#
#-------------------------------------------------

QT       += core gui widgets network

CONFIG += qxt
QXT += core widgets

TARGET = Qelly
TEMPLATE = app
CONFIG += precompile_header

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

CONFIG(static) {
    win32-g++ {
        LIBS += -static-libgcc -static-libstdc++
    }
}

TRANSLATIONS += UJ_Translate.ts
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
    View_p.cpp

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
    View_p.h

RESOURCES += \
    GeneralResources.qrc

FORMS += \
    PreferencesGeneral.ui \
    PreferencesFont.ui \
    PreferencesColor.ui
