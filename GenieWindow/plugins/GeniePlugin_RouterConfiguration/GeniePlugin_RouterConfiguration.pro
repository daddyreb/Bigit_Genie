# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core gui xml network widgets printsupport
TARGET = ../../../bin/GeniePlugin_RouterConfiguration
TEMPLATE = lib
CONFIG += plugin uitools

SUBDIRS += inc

SOURCES += GeniePlugin_RouterConfiguration.cpp \
    src/QGenieRCWirelessObject.cpp \
    src/QProgressIndicator.cpp \
    src/QGenieGaccObject.cpp \
    src/QGenieTimelineLocker.cpp \
    src/QGenieTimeGenerator.cpp \
    src/QGenieDelayProcessLogic.cpp \
    src/QPiObject.cpp \
    src/QGenieFupdateObject.cpp \
    src/QSetTextHelper.cpp \
    src/QGenieQRCodeManager.cpp \
    src/QGenieRCTopologyObject.cpp \
    src/QGenieTopoItems.cpp \
    src/QGenieRCLackDataGetter.cpp \
    src/QGenieQosObject.cpp \
    src/QGenieRebootObject.cpp

HEADERS += GeniePlugin_RouterConfiguration.h \
    src/QGenieRCWirelessObject.h \
    src/QProgressIndicator.h \
    src/QGenieGaccObject.h \
    src/QGenieTimelineLocker.h \
    src/QGenieTimeGenerator.h \
    src/QGenieDelayProcessLogic.h \
    src/QPiObject.h \
    src/QGenieFupdateObject.h \
    src/QSetTextHelper.h \
    src/QGenieQRCodeManager.h \
    src/QGenieRCTopologyObject.h \
    src/QGenieTopoItems.h \
    src/QGenieRCLackDataGetter.h \
    src/QGenieQosObject.h \
    src/QGenieRebootObject.h


include (../../include/genie2_interface/genie2_interface.pri)
include (../../include/genie2_netgear/genie2_netgear.pri)
include (../../include/genie2_routeranim/genie2_routeranim.pri)
include (../genie2_qrcode/genie2_qrcode.pri)
include (ReadyShare/readyshare.pri)
include (TrafficMeter/trafficmeter.pri)


INCLUDEPATH += . \
    ./src \
    ./TrafficMeter

#DESTDIR = ../debug

win32:LIBS += -L"../../bin" -lqrcode
DEFINES += USE_READYSHARE \
    USE_SUBPLUGINS \
    USE_TRAFFICMETER \
    USE_5GFUNC \
    USE_WIRELESS_PARENT_UI \
    USE_QOSFUNC


DEFINES += GENIE2_TRAY


