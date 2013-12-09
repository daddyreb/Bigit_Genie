TEMPLATE = lib
QT     += widgets
CONFIG += plugin debug_and_release uitools
#x86 ppc
TARGET = ../../../bin/GeniePlugin_ParentalControl
RESOURCES = res/resources.qrc
#DEFINES += LPCMANAGER_ENABLE_LOGGER
win32-msvc*: 
else:PRECOMPILED_HEADER = code/pch.h

HEADERS = \
	code/AsyncOp.h \
	code/common.h \
	code/coreapi.h \
	code/coreapi_p1.h \
	code/ext.h \
	code/logger.h \
	code/oputil.h \
	code/pages.h \
	code/webapi.h \
	code/webapi_p2.h \
	code/LPCMainFrame.h \
	code/LPCNavPage.h \
	code/LPCNavPopup.h \
	code/plugin.h \
	code/pch.h

SOURCES = \
	code/AsyncOp.cpp \
	code/common.cpp \
	code/coreapi.cpp \
	code/coreapi_p1.cpp \
	code/ext.cpp \
	code/logger.cpp \
	code/oputil.cpp \
	code/pages.cpp \
	code/webapi.cpp \
	code/webapi_p2.cpp \
	code/LPCMainFrame.cpp \
	code/LPCNavPage.cpp \
	code/LPCNavPopup.cpp \
	code/plugin.cpp

QT += network

INCLUDEPATH +=  . code

include (../../include/genie2_interface/genie2_interface.pri)
