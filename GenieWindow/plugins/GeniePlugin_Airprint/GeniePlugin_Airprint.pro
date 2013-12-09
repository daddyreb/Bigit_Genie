# -------------------------------------------------
# Project created by QtCreator 2011-11-16T14:36:10
# -------------------------------------------------
TARGET = ../../../bin/GeniePlugin_Airprint
TEMPLATE = lib
QT     += widgets \
          printsupport
CONFIG += plugin
SOURCES += src/GeniePlugin_Airprint.cpp \
    src/airprintwidget.cpp \
    src/airprintbackgroud.cpp \
    src/QProgressIndicator.cpp
HEADERS += src/GeniePlugin_Airprint.h \
    src/version.h \
    src/airprintwidget.h \
    src/airprintbackgroud.h \
    inc/genie_interface/date.h \
    inc/multilanguage_def/LanguageAirprint.h \
    src/QProgressIndicator.h
FORMS += ui/airprintwidget.ui
INCLUDEPATH += ./inc \
    ./inc/genie_interface \
    ./inc/multilanguage_def \
    ./inc/plugin_ui_def \
    ./src
RESOURCES += res/resource.qrc
include (../../include/genie2_interface/genie2_interface.pri)
LIBS += -lWinspool
