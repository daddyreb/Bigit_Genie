# -------------------------------------------------
# Project created by QtCreator 2010-05-24T09:36:30
# -------------------------------------------------
QT += core \
    gui
TARGET = ../../../bin/DiagnosePlugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += diagnoseplugin.cpp
HEADERS += DiagnoseInterface.h \
    diagnoseplugin.h
LIBS += ../../bin/DiagnoseDll.lib

INCLUDEPATH += ../DiagnoseDll/inc
