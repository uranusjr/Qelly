#-------------------------------------------------
#
# Project created by QtCreator 2011-10-05T21:59:37
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = TelnetTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../src

SOURCES += main.cpp \
    TelnetTester.cpp \
    ../../src/Telnet.cpp \
    ../../src/AbstractConnection.cpp

HEADERS += \
    TelnetTester.h \
    ../../src/Telnet.h \
    ../../src/AbstractConnection.h
