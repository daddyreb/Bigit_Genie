# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core gui xml network \
      widgets
TARGET = ../../../bin/GeniePlugin_Ui
TEMPLATE = lib
CONFIG += plugin uitools

SOURCES += GeniePlugin_Ui.cpp \ 
    src/QGenieButtonBig.cpp \
    src/QGenieUiObject.cpp \
    src/QGenieAdvStateMachine.cpp \ 
    src/QGenieTitleUpdate.cpp \
    src/QGenieModalDialog.cpp
HEADERS += GeniePlugin_Ui.h \
    src/QGenieButtonBig.h \
    src/QGenieUiObject.h \
    src/language_Ui.h \
    src/QGenieAdvStateMachine.h \ 
    src/QGenieTitleUpdate.h \
    src/QGenieModalDialog.h


include (../../include/genie2_interface/genie2_interface.pri)
include (../SvtNetworkTool/SvtNetworkTool.pri)
include (./genie2_waterwindow/genie2_waterwindow.pri)
include (../../include/genie2_routeranim/genie2_routeranim.pri)
include (./genie2_tray/genie2_tray.pri)

INCLUDEPATH += . \
    ./src \
	../SvtNetworkTool \
	../SvtNetworkTool\libs\msvc
	
macx:HEADERS +=	src/AppMenuHandler.h \
        #src/QGeniePrefrencesPanel.h \
		src/QGeniePrivilegeProcess.h
macx:SOURCES += src/AppMenuHandler.cpp \
        #src/QGeniePrefrencesPanel.cpp \
		src/QGeniePrivilegeProcess.cpp

macx:LIBS += -framework Security
macx:DEFINES += USE_DIRTY_FOCUS_CLEARER

FORMS += ui/QGenieButtonBig.ui \
    ui/QGenieModalDialog.ui

win32:
{
DEFINES += _WINDOWS WIN32_LEAN_AND_MEAN
LIBS += -L"../../bin" \
-lws2_32 \
-lSvtNetworkTool
}

#DEFINES += DBG_MODE
DEFINES += GENIE2_TRAY
DEFINES += USE_DBG_STACK







