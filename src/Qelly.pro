#-------------------------------------------------
#
# Project created by QtCreator 2011-10-04T01:19:38
#
#-------------------------------------------------

QT       += core gui network

TARGET = Qelly
TEMPLATE = app

TRANSLATIONS += UJ_Translate.ts

SOURCES += main.cpp \
    MainWindow.cpp \
    SharedMenuBar.cpp \
    Globals.cpp \
    Terminal.cpp \
    Encodings.cpp \
    AbstractConnection.cpp \
    Site.cpp \
    Ssh.cpp \
    Telnet.cpp \
    TabWidget.cpp

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
    Wrapper.h

RESOURCES += \
    GeneralResources.qrc
