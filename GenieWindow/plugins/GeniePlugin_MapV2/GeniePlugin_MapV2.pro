#-------------------------------------------------
#
# Project created by QtCreator 2011-08-31T15:58:42
#
#-------------------------------------------------

QT       += core gui xml network widgets

TARGET = ../../../bin/GeniePlugin_Map
TEMPLATE = lib
CONFIG += plugin uitools

macx {
#CONFIG += x86 ppc
#QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.6.sdk
#QMAKE_MACOSX_DEPLOYMENT_TARGET=10.5
}
GENIE_RELEASE {
} else {
macx:DESTDIR = ../../bin
}

TEMPLATE += fakelib
TEMPLATE -= fakelib

INCLUDEPATH += ./dlna/src \
            ./dlna/src/Core \
            ./dlna/src/DLNACore \
            ./dlna/src/System \
            ./dlna/src/DLNATest
            ./dlna/src/System/Win32

DEFINES += NPT_CONFIG_ENABLE_LOGGING

win32:LIBS += -L../../bin -lSvtNetworkTool -ltopology  -lhowl -ldlna
#else:LIBS +=  -L../topology/Build/MacOS -l$$TOPOLOGY_STATIC_LIBNAME

win32:LIBS += -lws2_32 -lShell32 -lCOMDLG32


DEFINES += USE_NODE_ANIMATION \
        #DELAY_CURRENTSETTING_CMD \
        REBOUNCE_FOR_RAISE \
        SIMPLE_TOPLEFT_TEXT

SOURCES += \
    src/GeniePlugin_MapV2.cpp \
    src/mapuitilities.cpp \
    src/device.cpp \
    src/netscanningcontroller.cpp \
    src/topologynode.cpp \
    src/topologyedge.cpp \
    src/topologyview.cpp \
    src/networkmapview.cpp \
    src/devicedetailview.cpp \
    src/devicedetailviewframe.cpp \
    src/devicedetailviewmanager.cpp \
    src/newdevicenotifywidget.cpp \
    src/newdevicenotifymanager.cpp \
    src/netcardspanel.cpp \
    src/QProgressIndicator.cpp \
    src/waittingpanel.cpp \
    src/detailviewcontentwidget.cpp \
    src/blockdevicemanager.cpp \
    src/puttyfuncs.cpp \
    src/blockdeviceconrolpanel.cpp \
    src/QGenieMapDbgWObject.cpp \
    src/udt/window.cpp \
    src/udt/queue.cpp \
    src/udt/packet.cpp \
    src/udt/md5.cpp \
    src/udt/list.cpp \
    src/udt/epoll.cpp \
    src/udt/core.cpp \
    src/udt/common.cpp \
    src/udt/channel.cpp \
    src/udt/ccc.cpp \
    src/udt/cache.cpp \
    src/udt/buffer.cpp \
    src/udt/api.cpp \
    src/newfilenotifywidget.cpp \
    src/newsendfilenotifywidget.cpp \
    src/browser.cpp \
    src/howlpublish.cpp \
    src/DNSServices.c \
    src/dnsservice.cpp \
    src/UdtCore.cpp \
    src/udtproxy.cpp \
    src/filenotifymanager.cpp \
    src/sendthread.cpp \
    src/msgnotifywidget.cpp \
    dlna/Src/DLNATest/DLNAProxy.cpp \ 
    src/UdtFile.cpp

HEADERS += \
    src/GeniePlugin_MapV2.h \
    inc/mapdevicetype.h \
    src/mapuitilities.h \
    src/device.h \
    src/netscanningcontroller.h \
    src/topologynode.h \
    src/topologyedge.h \
    src/topologyview.h \
    src/networkmapview.h \
    src/devicedetailview.h \
    src/devicedetailviewframe.h \
    src/devicedetailviewmanager.h \
    src/newdevicenotifywidget.h \
    src/newdevicenotifymanager.h \
    src/netcardspanel.h \
    src/QProgressIndicator.h \
    src/waittingpanel.h \
    src/detailviewcontentwidget.h \
    src/blockdevicemanager.h \
    src/puttyfuncs.h \
    src/blockdeviceconrolpanel.h \
    src/QGenieMapDbgWObject.h \
    src/udt/window.h \
    src/udt/udt.h \
    src/udt/queue.h \
    src/udt/packet.h \
    src/udt/md5.h \
    src/udt/list.h \
    src/udt/epoll.h \
    src/udt/core.h \
    src/udt/common.h \
    src/udt/channel.h \
    src/udt/ccc.h \
    src/udt/cache.h \
    src/udt/buffer.h \
    src/udt/api.h \
    src/udt/window.h \
    src/udt/udt.h \
    src/udt/queue.h \
    src/udt/packet.h \
    src/udt/md5.h \
    src/udt/list.h \
    src/udt/epoll.h \
    src/udt/core.h \
    src/udt/common.h \
    src/udt/channel.h \
    src/udt/ccc.h \
    src/udt/cache.h \
    src/udt/buffer.h \
    src/udt/api.h \
    src/udt/wspiapi.h \
    src/newfilenotifywidget.h \
    src/newsendfilenotifywidget.h \
    src/browser.h \
    src/howlpublish.h \
    src/mDNSPlatform.h \
    src/DNSServices.h \
    src/mDNSPlatformFunctions.h \
    src/mDNSDebug.h \
    src/mDNSClientAPI.h \
    src/mDNSServant.h \
    src/dnsservice.h \
    src/Win32/win32_mdns.h \
    src/UdtCore.h \
    src/cc.h \
     src/udtproxy.h \
    src/filenotifymanager.h \
    src/sendthread.h \
    src/msgnotifywidget.h \
    dlna/Src/DLNATest/DLNAProxy.h \
    src/UdtFile.h

INCLUDEPATH += . \
    ./src \
    ./src/udt \
    ./inc \
    ../topology/inc \
    ../SVTWin32Utils

win32-g++: {
 INCLUDEPATH += ../SvtNetworkTool/libs/mingw
 INCLUDEPATH += ../SvtNetworkTool/libs/mingw/openssl

 DEFINES += __MINGW__ WIN32 _WIN32_WINNT=0x0500

 INCLUDEPATH += ./howl/include \
    ./howl/mDNSResponder

 LIBS += -lIphlpapi
}
win32-msvc*: {
 INCLUDEPATH += ../SvtNetworkTool/libs/msvc \
				../SvtNetworkTool/libs/msvc/openssl \
				../SvtNetworkTool \
				./howl \
				./howl/include \
				./howl/howl \
				./howl/mDNSResponder

 LIBS += -lIphlpapi
 DEFINES += _WINDOWS LEGACY_WIN32 WIN32_LEAN_AND_MEAN NO_UDT_DLL NDEBUG

}

#add by caiyujun
win32 {
    contains(TEMPLATE, lib):
}
#add end
	
include (../../include/genie2_interface/genie2_interface.pri)
include (../../include/genie2_routeranim/genie2_routeranim.pri)
include (../../include/genie2_netgear/genie2_netgear.pri)
include (../SvtNetworkTool/SvtNetworkTool.pri)

DEFINES += PLUGIN_TARGET \
    RETRANSLATE_PROPERTYS

DEFINES+=BLOCK_DEVICE
BLOCK_DEVICE_DBG:DEFINES+=BLOCK_DEVICE_DBG
#POWERLINE:DEFINES += POWERLINE
ces:DEFINES += POWERLINE_REVISION
win32:DEFINES += USE_RDP
win32:DEFINES += USE_PUTTY

#ENABLE_POWERLINE{

DEFINES += ENABLE_POWERLINE ENABLE_SMARTWIFIWIZARD RUN_IN_MAINFRAME

include(./powerline/powerline.pri)
#}
