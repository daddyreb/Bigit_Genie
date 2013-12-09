#-------------------------------------------------
#
# Project created by QtCreator 2011-03-11T09:44:23
#
#-------------------------------------------------

TARGET = slib
TEMPLATE = lib
CONFIG += staticlib
CONFIG += plugin

SOURCES += QGenieBasePluginLib.cpp \
    QGenieBaseWidget.cpp
HEADERS += inc/QGenieBasePluginLib.h \
    inc/QGenieBaseWidget.h \
    uuid.h


INCLUDEPATH += ./inc \
    ./inc/genie_interface

DESTDIR = ..
