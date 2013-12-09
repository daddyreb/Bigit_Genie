TEMPLATE = app
CONFIG += debug_and_release precompile_header uitools x86 ppc

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
	code/env.h \
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
	code/env.cpp

PRECOMPILED_HEADER = code/pch.h

RESOURCES = res/env.qrc

QT += network
