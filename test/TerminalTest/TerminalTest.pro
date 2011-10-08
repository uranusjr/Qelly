#-------------------------------------------------
#
# Project created by QtCreator 2011-10-08T05:36:03
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = TerminalTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../Test

SOURCES += main.cpp \
    ../../src/Terminal.cpp \
    ../../src/Site.cpp \
    ../../src/Telnet.cpp \
    ../../src/AbstractConnection.cpp \
    TerminalTester.cpp

HEADERS += \
    ../../src/Terminal.h \
    ../../src/YLTerminal.h \
    ../../src/UJCommonDefs.h \
    ../../src/Globals.h \
    ../../src/Site.h \
    ../../src/Telnet.h \
    ../../src/AbstractConnection.h \
    TerminalTester.h \
    ../Test/UJQxTestUtilities.h







