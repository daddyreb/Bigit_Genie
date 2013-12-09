# -------------------------------------------------
# Project created by QtCreator 2010-06-09T13:46:21
# -------------------------------------------------
QT += core \
    gui \
    xml \
    network \
    widgets
TARGET = ../../../bin/InnerPlugin_WirelessExport
#macx:DESTDIR = ../../bin
# GENIE_RELEASE:
# else:DESTDIR = ../../debug/main_frame/bin
TEMPLATE = lib
CONFIG += plugin
#macx:CONFIG += x86 ppc
INCLUDEPATH += ../../include/genie2_interface \
    ../WSetupApiPlugin \
    ./soap \
    .

include (../../include/genie2_interface/genie2_interface.pri)

HEADERS += QGenieWirelesssetupWizard.h \
    QGenieWirelessPromptPage.h \
    QGenieWelcomePage.h \
    QGenieResultPage.h \
    QGenieInsertFlashDiskPage.h \
    GeniePlugin_WSetupPlugin.h \
    QProgressIndicator.h \
    QGenieProgressPanel.h \
    QGenieExport2DiskFileThread.h

SOURCES += QGenieWirelesssetupWizard.cpp \
    QGenieWirelessPromptPage.cpp \
    QGenieWelcomePage.cpp \
    QGenieResultPage.cpp \
    QGenieInsertFlashDiskPage.cpp \
    GeniePlugin_WSetupPlugin.cpp \
    QProgressIndicator.cpp \
    QGenieProgressPanel.cpp \
    QGenieExport2DiskFileThread.cpp


macx{
    SOURCES += QGenieMacOSXApi.cpp \
    QGenieCodec.cpp

    HEADERS += QGenieMacOSXApi.h \
    QGenieCodec.h
}

RESOURCES += QGenieWirelessExport.qrc \
    GenieWirelessConfig.qrc

macx:DEFINES += PLATFORM_MACOSX
win32:DEFINES += PLATFORM_WIN32
DEFINES += WIRELESSIMPORT_MACX_SUPPORT
