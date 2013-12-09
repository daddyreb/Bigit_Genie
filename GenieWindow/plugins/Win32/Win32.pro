

#CONFIG += console
TEMPLATE = lib
CONFIG += staticlib
DESTDIR =../Win32
INCLUDEPATH += ../SVTWin32Utils \
    ../topology/inc \
    ../topology/src \
    ../topology/src/soap

QT += network xml widgets

DEFINES += TEST_VALIDATE \
    QT_NO_DEBUG_OUTPUT \
    NOTIFY_ADDED_DEVICE \
    REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
win32:DEFINES += RETRY_BEFORE_OFFLINE
win32:DEFINES += USE_NETSCAN_HELPER_THREAD
macx:DEFINES += FIX_FIX_MACOS_10_5_BUG
#DEFINES += NO_BLOCK_TCP

HEADERS += \
    ../topology/src/SvtQtUtils.h \
    ../topology/inc/topology.h \
    ../topology/src/SvtReferences.h \
    ../topology/src/SvtCore.h \
    ../topology/src/SvtAvlTree.h \
    ../topology/src/SvtNetbiosMethod.h \
    ../topology/src/SvtNetscanMethod.h \
   ../topology/src/SvtUpnpMethod.h \
    ../topology/src/SvtList.h \
    ../topology/src/SvtProgardThread.h \
    ../topology/src/SvtControlProxy.h \
    ../topology/src/SvtSeedFactory.h \
    ../topology/src/SvtRunnables.h \
    ../topology/src/defines.h \
    ../topology/src/SvtUpnpHelper.h \
    ../topology/src/SvtNetgearSpecificMethod.h \
    ../topology/src/SvtNetgearSpecificHelper.h \
   # ../../src/soap/qtsoap.h \
   # ../../src/soap/netgearsoapschedule.h \
   # ../../src/soap/netgearsoapapi.h \
    ../topology/src/SvtAfpMethod.h \
    ../topology/src/netgear_info/SvtNetgearInfo.h \
    ../topology/src/SvtRdpMethod.h \
    ../topology/src/SvtPuttyMethod.h \
    ../topology/src/SvtThreadMother.h \
    ../topology/src/SvtTracer.h \
    ../topology/src/GenieHttp.h



SOURCES += \
    ../topology/src/SvtQtUtils.cpp \
    ../topology/src/SvtCore.cpp \
    ../topology/src/SvtNetbiosMethod.cpp \
    ../topology/src/SvtNetscanMethod.cpp \
    ../topology/src/SvtUpnpMethod.cpp \
    ../topology/src/SvtProgardThread.cpp \
    ../topology/src/SvtControlProxy.cpp \
    ../topology/src/SvtSeedFactory.cpp \
    ../topology/src/SvtRunnables.cpp \
    ../topology/src/SvtUpnpHelper.cpp \
    ../topology/src/SvtNetgearSpecificMethod.cpp \
    ../topology/src/SvtNetgearSpecificHelper.cpp \
    #../../src/soap/qtsoap.cpp \
    #../../src/soap/netgearsoapschedule.cpp \
    #../../src/soap/netgearsoapapi.cpp \
    ../topology/src/SvtAfpMethod.cpp \
    ../topology/src/netgear_info/SvtNetgearInfo.cpp \
    ../topology/src/SvtRdpMethod.cpp \
    ../topology/src/SvtPuttyMethod.cpp \
    ../topology/src/SvtThreadMother.cpp \
    ../topology/src/SvtTracer.cpp \
    ../topology/src/GenieHttp.cpp

macx:HEADERS += ../topology/src/SvtunixProcessHelper.h
macx:SOURCES += ../topology/src/SvtunixProcessHelper.cpp


TARGET = Topology

include (../SvtNetworkTool/SvtNetworkTool.pri)
DEFINES+=BLOCK_DEVICE
ces:DEFINES += POWERLINE_REVISION
BLOCK_DEVICE_DBG:DEFINES+=BLOCK_DEVICE_DBG
win32:DEFINES += USE_RDP
win32:DEFINES += USE_PUTTY
