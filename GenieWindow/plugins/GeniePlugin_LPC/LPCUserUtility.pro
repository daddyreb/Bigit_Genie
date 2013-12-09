TEMPLATE = app
CONFIG += debug_and_release uitools

HEADERS = \
	code/AsyncOp.h \
	code/common.h \
	code/coreapi.h \
	code/coreapi_p1.h \
	code/ext.h \
	code/logger.h \
	code/oputil.h \
	code/webapi.h \
	code/webapi_p2.h \
	code/UUApp.h \
	code/UUPages.h \
	code/pch.h

SOURCES = \
	code/AsyncOp.cpp \
	code/common.cpp \
	code/coreapi.cpp \
	code/coreapi_p1.cpp \
	code/ext.cpp \
	code/logger.cpp \
	code/oputil.cpp \
	code/webapi.cpp \
	code/webapi_p2.cpp \
	code/UUApp.cpp \
	code/UUPages.cpp

PRECOMPILED_HEADER = code/pch.h

RESOURCES = res/LPCUserUtility.qrc

DEFINES += LPC_HAVE_GATEWAY_MAC

QT += network

win32-msvc*:{
	LIBS += iphlpapi.lib
	DEFINES += UU_USE_TRAY_MENU UU_TRAY_ICON_AUTO
}

win32-g++:{
	LIBS += -liphlpapi
	DEFINES += UU_USE_TRAY_MENU UU_TRAY_ICON_AUTO
}

macx:{
	CONFIG += x86 ppc
	ICON = res/images/LPCUserUtility.icns
	QMAKE_INFO_PLIST = LPCUserUtility.Info.plist
	DEFINES += UU_USE_APP_MENU
}
