# -------------------------------------------------
# Project created by QtCreator 2013-7-29T16:27:10
# -------------------------------------------------
TARGET = ../../../bin/GeniePlugin_SmartWifi
TEMPLATE = lib
QT     += widgets network xml
CONFIG += plugin

INCLUDEPATH += ./inc \
    ./inc/genie_interface \
    ./inc/multilanguage_def \
    ./inc/plugin_ui_def \
    ./src

include (../../include/genie2_interface/genie2_interface.pri)


SOURCES += src\GeniePlugin_SmartWifi.cpp \
           src\SmartWifiWidget.cpp \
           src\SmartWifiParser.cpp \
		   src\SmartWifiConnector.cpp \
		   src\GeniePlugin_SmartWifi.cpp \
		   src\LaunchThread.cpp \
		   src\SmartWifiConnector.cpp
		   
HEADERS += src\GeniePlugin_SmartWifi.h \
           src\SmartWifiWidget.h \
           src\language_SmartWifi.h \
		   src\smartWifiProtocol.h \
		   src\ConnectorTranaction.h \
		   src\ConnectorTranaction.h \
		   src\LaunchThread.h \
		   src\SmartWifiConnector.h
FORMS += ./smartwifiwidget.ui