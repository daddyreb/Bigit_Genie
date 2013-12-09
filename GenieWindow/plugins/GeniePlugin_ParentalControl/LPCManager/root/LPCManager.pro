TEMPLATE = app

QT += network

CONFIG += uitools

RESOURCES = res/LPCManager.qrc

HEADERS += \
	code/app_config.h \
	code/AsyncOp.h \
	code/app.h \
	code/logger.h \
	code/ctrl.h \
	code/ext.h \
	code/oputil.h \
	code/webapi.h \
#	code/webapi_p1.h \
	code/webapi_p2.h \
	code/coreapi.h \
	code/coreapi_p1.h \
#	code/router.h \
	code/common.h \
	code/pch.h

SOURCES += \
	code/AsyncOp.cpp \
	code/app.cpp \
	code/logger.cpp \
	code/ctrl.cpp \
	code/ext.cpp \
	code/oputil.cpp \
	code/webapi.cpp \
#	code/webapi_p1.cpp \
	code/webapi_p2.cpp \
	code/coreapi.cpp \
	code/coreapi_p1.cpp \
#	code/router.cpp \
	code/common.cpp \
	code/main.cpp

DEFINES += QT_NO_CAST_FROM_ASCII ENABLE_DEBUG_UI
build_pass:CONFIG(debug,debug|release) {
DEFINES += LPCMANAGER_ENABLE_LOGGER
#} else {
}
	
win32-msvc*:{
	LIBS += iphlpapi.lib
}

win32-g++:{
	LIBS += -liphlpapi
}

win32:{
	RC_FILE = res/LPCManager.rc
}

macx:{
	CONFIG += x86 ppc
	ICON = res/images/LPCManager.icns
}
