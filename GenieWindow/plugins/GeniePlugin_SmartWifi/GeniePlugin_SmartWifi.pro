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
		   src\WhiteUrlTableWidget.cpp \
		   src\SmartWifiParser.cpp \
		   src\SmartWifiConnector.cpp \
		   src\LineEdit.cpp \
		   src\LaunchThread.cpp \ 
		   src\WaittingDlg.cpp
		   
HEADERS += src\GeniePlugin_SmartWifi.h \
           src\SmartWifiWidget.h \
           src\language_SmartWifi.h \
		   src\WhiteUrlTableWidget.h \
		   src\version.h \
		   src\smartWifiProtocol.h \
		   src\SmartWifiParser.h \
		   src\SmartWifiConnector.h \
		   src\LineEdit.h \
		   src\LaunchThread.h \
		   src\global.h \
		   src\GetWhiteListThread.h \
		   src\AddWhiteUrlThread.h \
		   src\WaittingDlg.h
		   
FORMS += smartwifiwidget.ui
RESOURCES = smartwifi.qrc  
		   
		   
		   