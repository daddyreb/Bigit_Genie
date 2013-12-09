# -------------------------------------------------
# Project created by QtCreator 2010-09-01T10:24:45
# -------------------------------------------------
macx:QT += xml network
win32:QT += gui xml network widgets
GENIE_RELEASE: {
} else {
macx:DESTDIR = ../../bin
}
TARGET = ../../../bin/InternetDaemon
TEMPLATE = app
CONFIG   -= app_bundle
#macx:CONFIG += x86 ppc
SOURCES += main.cpp \
    ../../include/Internet_IPC/ChannelProducer.cpp \
    ../../include/Internet_IPC/ChannelConsumer.cpp \
    InternetDaemonMsgProcessor.cpp \
    soap/qtsoap.cpp \
    soap/netgearsoapschedule.cpp \
    soap/netgearsoapapi.cpp
HEADERS += ../../include/Internet_IPC/IPCCommon.h \
    ../../include/Internet_IPC/ChannelProducer.h \
    ../../include/Internet_IPC/ChannelConsumer.h \
    ../../include/Internet_IPC/IChannelMessageProcessor.h \
    InternetDaemonMsgProcessor.h \
    ../../include/Internet_IPC/InternetDaemonDefs.h \
    soap/netgearsoapschedule.h \
    soap/netgearsoapapi.h \
    soap/qtsoap.h
INCLUDEPATH += ../../include/Internet_IPC \
    ./soap \
    .
win32:INCLUDEPATH +=     ../DiagnosePlugin
macx:INCLUDEPATH += ../lib

win32:SOURCES += QGenieWinFixThread.cpp
win32:HEADERS += QGenieWinFixThread.h

macx:SOURCES += QGenieMacxFixThread.cpp \
    SingleInstance.c
macx:HEADERS += QGenieMacxFixThread.h \
    SingleInstance.h

macx:DEFINES += INTERNET_DAEMON_UNIX
win32:DEFINES += ENABLE_CONFIGDNS_MANUALLY
DEFINES += TESTSOAPAPI
