# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core \
    gui \
    widgets
TARGET = ../../../bin/GeniePlugin_Resource
TEMPLATE = lib
CONFIG += plugin

SOURCES += GeniePlugin_Resource.cpp 
HEADERS += GeniePlugin_Resource.h

RESOURCES += images/images.qrc 	\
                images/route/route.qrc \
		i18n/i18n.qrc \
                ui/ui.qrc \
    map/map.qrc \
        images/readyshare/readyshare.qrc \
		fontsetting/fontsetting.qrc
macx:RESOURCES += i18n/i18n_mainframe.qrc
include (../../include/genie2_interface/genie2_interface.pri)
INCLUDEPATH += . \
    ../../src/inc \
    ./inc/genie_interface
#DESTDIR = ../debug
