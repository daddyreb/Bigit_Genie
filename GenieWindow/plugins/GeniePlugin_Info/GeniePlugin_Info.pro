# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core \
    gui \
    xml \
    network \
    testlib
TARGET = GeniePlugin_Info
GENIE_RELEASE:


GENIE_RELEASE:
else:DESTDIR = ../../../trunk/debug


TEMPLATE = lib
CONFIG += plugin
SOURCES += GeniePlugin_Info.cpp \
    netgearinfo.cpp \
    getinformation.cpp \
    netscanthread.cpp
HEADERS += GeniePlugin_Info.h \
    inc/plugin_ui_def/plugin_ui_def.h \
    inc/genie_interface/ITrayManager.h \
    inc/genie_interface/IToyManager.h \
    inc/genie_interface/ISoapManager.h \
    inc/genie_interface/IPluginInf.h \
    inc/genie_interface/IManagerHandler.h \
    inc/genie_interface/IInfoManager.h \
    inc/genie_interface/IClickManager.h \
    inc/genie_interface/IUiManager.h \
    inc/genie_interface/ILanguageManager.h \
    inc/genie_interface/bbt_paras.h \
    netgearinfo.h \
    getinformation.h \
    inc/genie_interface/date.h \
    netscanthread.h
INCLUDEPATH += . \
    ./inc \
    ./inc/genie_interface \
    ./inc/plugin_ui_def \
    ./inc/multilanguage_def
RESOURCES += 
FORMS += 
