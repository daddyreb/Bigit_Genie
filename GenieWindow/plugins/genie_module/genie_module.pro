TARGET = ../../../bin/Genie
TEMPLATE = lib
QT     += widgets
CONFIG += plugin
CONFIG += debug_and_release
#PRECOMPILED_HEADER = src/inc/stable.h

win32:LIBS += -lws2_32

INCLUDEPATH += . \
    src/inc \
    src/inc/dll_hearder \
    src/inc/genie_interface \
    src/uipage \
	src/usages \
    src/uiframe \
    src/svp \
    src/interface \
    src/interface2 \
    src/inc/plugin_ui_def \
    src/soap \
    src/inc/launch_interface \
    src/launch_interface \
    src/inc/multilanguage_def \
    src/share \
    src/network_util

HEADERS += src/launch_interface/QGenieLaunchInf.h \
    src/inc/Singleton.h \
    src/usages/QGenieAllSingletonClasses.h \
    src/inc/stable.h \
 #   src/inc/launch_interface/ILaunchInf.h \
 #   src/inc/launch_interface/ILaunchManager.h \
    src/interface2/QGenieInterface2Helper.h \
    src/interface2/QGenieHandlerInf.h \
    src/interface2/QGenieInterface2LanguageManager.h \
    src/interface2/QGenieInterface2SubscribedMessageManager.h \
    src/interface2/QGenieInterface2LogManager.h
SOURCES += src/usages/QGenieAllSingletonClasses.cpp \
    src/launch_interface/QGenieLaunchInf.cpp \
    src/interface2/QGenieInterface2Helper.cpp \
    src/interface2/QGenieHandlerInf.cpp \
    src/interface2/QGenieInterface2LanguageManager.cpp \
    src/interface2/QGenieInterface2SubscribedMessageManager.cpp \
    src/interface2/QGenieInterface2LogManager.cpp
include (../../plugins/SvtNetworkTool/SvtNetworkTool.pri)
include (../../include/genie2_interface/genie2_interface.pri)
include (../../include/genie2_settings/genie2_settings.pri)
include (src/genie2_singlestart/genie2_singlestart.pri)
include (../../include/genie2_netgear/genie2_netgear_realize.pri)

RESOURCES += images/images_splash.qrc
DEFINES += TEST_UI
DEFINES += GENIE2_MAINFRAME
DEFINES += USE_LAZYLOAD

#GENIE_RELEASE:DEFINES += NO_ASSERT
#TEMPLATE += fakelib

INCLUDEPATH += ./ \
	../SvtNetworkTool \
	../SvtNetworkTool\libs\msvc
win32:
{
	DEFINES += _WINDOWS  WIN32_LEAN_AND_MEAN  
	LIBS += -L"../../bin" \
	-lws2_32 \
	-lSvtNetworkTool
}

# PINGLIB_LIBNAME = $$qtLibraryTarget(pinglib)
# TOPOLOGYLIB_LIBNAME = $$qtLibraryTarget(topologylib)
TEMPLATE -= fakelib
QT += webkit \
    network \
    xml
macx:DESTDIR = ./debug
DEFINES += INTERFACE2
DEFINES += USE_DBG_STACK
