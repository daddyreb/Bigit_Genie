# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core \
    gui
TARGET = GeniePlugin_Test
TEMPLATE = lib
CONFIG += plugin

SOURCES += GeniePlugin_Test.cpp \
    QGenieTestForm.cpp
HEADERS += GeniePlugin_Test.h \
    QGenieTestForm.h

include (../src/qt_spy/qt_spy.pri)
include (../src/genie2_interface/genie2_interface.pri)
include (../src/genie2_routeranim/genie2_routeranim.pri)
include (../src/genie2_netgear/genie2_netgear.pri)
#include (../src/genie2_netgear/blockedsoap/blockedsoap.pri)

INCLUDEPATH += . \
    ../../src/inc \
    ./inc/genie_interface
#DESTDIR = ../debug

FORMS += \
    QGenieTestForm.ui
