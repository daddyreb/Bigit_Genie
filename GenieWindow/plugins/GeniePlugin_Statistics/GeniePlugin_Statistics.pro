# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core \
    gui \
    network \
    xml \
    widgets
TARGET = ../../../bin/GeniePlugin_Statistics

TEMPLATE = lib
CONFIG += plugin
SOURCES += ./src/GeniePlugin_Statistics.cpp \
    ./src/QGenieDataContainner.cpp \
    ./src/QGenieStaticInterface.cpp \
    ./src/QGenieHttpRequestManager.cpp
HEADERS += ./src/GeniePlugin_Statistics.h \
    inc/genie_interface/info_defs.h \
    inc/genie_interface/date.h \
    selfdef.h \
    version.h \
    ./src/QGenieDataContainner.h \
    ./src/QGenieStaticInterface.h \
    ./src/QGenieHttpRequestManager.h
INCLUDEPATH += ./inc \
    ../DiagnosePlugin \
    ./ \
	../SvtNetworkTool \
	../SvtNetworkTool\libs\msvc \
	../genie_module\src\inc\genie_interface \
	
win32:
{
DEFINES += WIN32_LEAN_AND_MEAN
LIBS += -L"../../bin" \
-lws2_32 \
-lSvtNetworkTool
}

include (../SvtNetworkTool/SvtNetworkTool.pri)
include (../../include/genie2_interface/genie2_interface.pri)
include (../../include/genie2_settings/genie2_settings.pri)
include (../../include/genie2_netgear/genie2_netgear.pri)

FORMS += ui/QGenieStaticInterface.ui
