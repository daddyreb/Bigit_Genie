#-------------------------------------------------
#
# Project created by QtCreator 2012-01-16T10:56:02
#
#-------------------------------------------------

QT       += core gui xml network widgets

TARGET = ../../../bin/InnerPlugin_Update
TEMPLATE = lib
CONFIG += plugin uitools


SOURCES += src/GeniePlugin_Update.cpp \
    src/QGenieUpdateObject.cpp \
    src/QSetTextHelper.cpp

HEADERS  += src/GeniePlugin_Update.h \
    version.h \
    src/QGenieUpdateObject.h \
    src/QSetTextHelper.h

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
	


include (../SvtNetworkTool/SvtNetworkTool.pri)
include (../../include/genie2_interface/genie2_interface.pri)
include (../../include/genie2_routeranim/genie2_routeranim.pri)
include (../../include/genie2_netgear/genie2_netgear.pri)


INCLUDEPATH += . 
