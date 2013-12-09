# -------------------------------------------------
# Project created by QtCreator 2010-05-25T16:17:33
# -------------------------------------------------
QT += core \
    gui \
    network \
    widgets
TARGET = ../../../bin/GeniePlugin_Internet
TEMPLATE = lib
CONFIG += plugin
GENIE_RELEASE: {
} else {
macx:DESTDIR = ../../bin
}
#macx:CONFIG += x86 ppc
#else:DESTDIR = ../trunk/debug

GENIE_DAEMON_CODE_DIR = $$PWD/../GenieDaemon/trunk/code/daemon

# GENIE_RELEASE:
# else:DESTDIR = ../../debug/main_frame/bin
INCLUDEPATH += ../../include/genie2_interface \
    ../../include/genie2_routeranim \
    ../DiagnosePlugin \
    ../../include/Internet_IPC \
    $$GENIE_DAEMON_CODE_DIR \
    .
macx:INCLUDEPATH += ../lib

include (../../include/genie2_interface/genie2_interface.pri)
include (../../include/genie2_routeranim/genie2_routeranim.pri)

HEADERS += QGenieVisibleNetworkWidget.h \
    QGenieRefreshStatusThread.h \
    QGeniePluginCableWidget.h \
    QGenieNetworkStatePanel.h \
    QGenieNetworkDiagnose.h \
    QGenieInputPasswordWidget.h \
    QGenieAvailableNetworkList.h \
    QGenieAvailableNetworkItem.h \
    GeniePlugin_Internet.h \
    QGenieRefreshVisibleNetworkThread.h \
    QProgressIndicator.h \
    QGenieProgressPanel.h \
    QGenieWlanKeyPersistent.h \
    QGenieSwitchonWlannicRadioWidget.h \
    QGenieDialogContainerFrame.h \
    QGenieDialogInnerWidget.h \
    ../../include/Internet_IPC/IPCCommon.h \
    ../../include/Internet_IPC/InternetDaemonDefs.h \
    ../../include/Internet_IPC/IChannelMessageProcessor.h \
    ../../include/Internet_IPC/ChannelProducer.h \
    ../../include/Internet_IPC/ChannelConsumer.h \
    InternetMsgProcessor.h \
    QGenieSpeedTest.h \
    QGenieDisclaimerWidget.h \
    QGenieRefreshInternetStateHandler.h \
    QGenieThreadTimer.h \
    QGenieConnectionStateDetecter.h \
    QGenieRestartRouterWidget.h \
	LookUpThread.h
win32:HEADERS += \
    Globals.h \
    QGenieNetMonitor.h \
    QGenieNetTrafficGraph.h \
    DnsQuery_Win.h \
    QGenieWzcMonitorThread.h

SOURCES += QGenieVisibleNetworkWidget.cpp \
    QGenieRefreshStatusThread.cpp \
    QGeniePluginCableWidget.cpp \
    QGenieNetworkStatePanel.cpp \
    QGenieNetworkDiagnose.cpp \
    QGenieInputPasswordWidget.cpp \
    QGenieAvailableNetworkList.cpp \
    QGenieAvailableNetworkItem.cpp \
    GeniePlugin_Internet.cpp \
    QGenieRefreshVisibleNetworkThread.cpp \
    QProgressIndicator.cpp \
    QGenieProgressPanel.cpp \
    QGenieWlanKeyPersistent.cpp \
    QGenieSwitchonWlannicRadioWidget.cpp \
    QGenieDialogContainerFrame.cpp \
    QGenieDialogInnerWidget.cpp \
    ../../include/Internet_IPC/ChannelProducer.cpp \
    ../../include/Internet_IPC/ChannelConsumer.cpp \
    InternetMsgProcessor.cpp \
    QGenieSpeedTest.cpp \
    QGenieDisclaimerWidget.cpp \
    QGenieRefreshInternetStateHandler.cpp \
    QGenieThreadTimer.cpp \
    QGenieConnectionStateDetecter.cpp \
    QGenieRestartRouterWidget.cpp \
	LookUpThread.cpp

win32:SOURCES += \
	QGenieNetMonitor.cpp \
        QGenieNetTrafficGraph.cpp \
        DnsQuery_Win.cpp \
        QGenieWzcMonitorThread.cpp

macx:SOURCES += QGenieChooseLocationWidget.cpp \
    QGeniePrivilegeProcess.cpp \
    VerifyDaemonHandler.cpp

macx:HEADERS += QGenieChooseLocationWidget.h \
    QGeniePrivilegeProcess.h \
    VerifyDaemonHandler.h

win32:RESOURCES += QGeniePluginInternet.qrc
macx:RESOURCES += QGeniePluginInternet_Mac.qrc
win32:DEFINES += GENIE_ENABLE_NET_MONITOR
win32:
{
	LIBS += -lDnsapi \
			-lole32 \
			-lShell32
}
macx:LIBS += -framework \
    Security # for QGeniePrivilegeProcess
	
win32:{
	win32-msvc*: {
		INCLUDEPATH += $$GENIE_DAEMON_CODE_DIR/../../3rdparty/msvc_std/include
	} else {
		LIBS += -lole32 -lShell32
	}

	HEADERS += \
		$$GENIE_DAEMON_CODE_DIR/ipc.h \

	SOURCES += \
		$$GENIE_DAEMON_CODE_DIR/proto.cpp \
		$$GENIE_DAEMON_CODE_DIR/ipc2_win32.cpp
}

macx:{
	DEFINES += GENIE_ENABLE_NET_MONITOR
	HEADERS += \
		QGenieNetMonitor.h \
		QGenieNetTrafficGraph.h

	SOURCES += \
		QGenieNetMonitor.cpp \
		QGenieNetTrafficGraph.cpp \
		$$GENIE_DAEMON_CODE_DIR/ipc2_unix.cpp \
		$$GENIE_DAEMON_CODE_DIR/proto.cpp
}
