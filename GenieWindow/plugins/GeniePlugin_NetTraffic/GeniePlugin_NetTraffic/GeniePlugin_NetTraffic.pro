# -------------------------------------------------
# Project created by QtCreator 2010-06-04T13:59:20
# -------------------------------------------------
QT += core \
    gui
TARGET = GeniePlugin_NetTraffic
TEMPLATE = lib
CONFIG += plugin
GENIE_RELEASE:
else:DESTDIR = D:\plugin\bin
SOURCES += GeniePlugin_NetTraffic.cpp \
    QGenie_MFNetTraffic.cpp \
    QGenie_qtOptions.cpp \
    QGenie_qtNetTraffic.cpp \
    QGenie_qtNetStat.cpp \
    QGenie_qtGraphColor.cpp \
    QGenieAboutThread.cpp
HEADERS += GeniePlugin_NetTraffic.h \
    Globals.h \
    QGenie_MFNetTraffic.h \
    QGenie_qtNetTraffic.h \
    QGenie_qtNetStat.h \
    QGenie_qtGraphColor.h \
    QGenie_qtOptions.h \
    inc/genie_interface/IUiManager.h \
    inc/genie_interface/ITrayManager.h \
    inc/genie_interface/IToyManager.h \
    inc/genie_interface/ISoapManager.h \
    inc/genie_interface/IPluginInf.h \
    inc/genie_interface/IManagerHandler.h \
    inc/genie_interface/ILanguageManager.h \
    inc/genie_interface/IInfoManager.h \
    inc/genie_interface/IClickManager.h \
    inc/multilanguage_def/LanguageNetTraffic.h \
    inc/genie_interface/setting_def.h \
    QGenieAboutThread.h \
    inc/genie_interface/ILogManager.h \
    inc/genie_interface/date.h \
    selfdef.h \
    inc/genie_interface/info_defs.h \
    version.h
INCLUDEPATH += ../../src/inc \
    ./inc/genie_interface \
    ./inc/plugin_ui_def \
    ./inc/multilanguage_def \
    ./
LIBS += ExTable.lib \
    -liphlpapi \
    -lws2_32
RESOURCES += qtTraffic.qrc
