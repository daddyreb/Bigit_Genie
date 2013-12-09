# -------------------------------------------------
# Project created by QtCreator 2010-05-27T13:48:46
# -------------------------------------------------
QT       += core network widgets
TARGET = ../../../bin/DragonNetTool
TEMPLATE = lib
SOURCES += dragonnettool.cpp \
    getthread.cpp \
    getsystemdetails.cpp
HEADERS += interface/dragonnettool.h \
    winorlinux.h \
    getthread.h \
    getsystemdetails.h
INCLUDEPATH += ./interface
#DESTDIR = D:\Works\NewWorks\SiteviewTooles\debug

DEFINES += WIN32_LEAN_AND_MEAN DRAGONNETTOOL_LIB
# ifdef WIN32
LIBS += -lWS2_32
