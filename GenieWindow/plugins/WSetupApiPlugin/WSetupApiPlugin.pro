# -------------------------------------------------
# Project created by QtCreator 2010-06-02T17:21:37
# -------------------------------------------------
QT += core \
    gui
TARGET = ../../../bin/WSetupApiPlugin
TEMPLATE = lib
CONFIG += plugin
SOURCES += wsetupapiplugin.cpp
HEADERS += WSetupApiInterface.h \
    wsetupapiplugin.h
LIBS += ../../bin/WSetupDll.lib
INCLUDEPATH += ../WSetupDll
