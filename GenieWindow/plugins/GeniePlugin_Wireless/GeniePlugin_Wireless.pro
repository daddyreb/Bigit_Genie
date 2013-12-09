# -------------------------------------------------
# Project created by QtCreator 2010-06-21T11:14:31
# -------------------------------------------------
QT += core \
    gui \
    xml \
    network \
    widgets
TARGET = ../../../bin/GeniePlugin_Wireless
TEMPLATE = lib
GENIE_RELEASE: {
} else {
macx:DESTDIR = ../../bin
}
DEFINES += ENBABLE_UPDATEWIRELESS_REALTIME
#win32:
DEFINES += ENABLE_WIFIPROFILE_MANAGER
CONFIG += plugin
#macx:CONFIG += x86 ppc
SOURCES += GeniePlugin_Wireless.cpp \
    QGenieXLabelPanel.cpp \
    QGenieWirelessChannelView.cpp \
    QGenieVisibleNetworkWidget.cpp \
    QGenieInputPasswordWidget.cpp \
    QGenieHistogramWidget.cpp \
    QGenieCurChannelWidget.cpp \
    QGenieColumnWidget.cpp \
    QGenieColumnUnit.cpp \
    QGenieColumnPanel.cpp \
    QGenieColumnLabel.cpp \
    QGenieAvailableNetworkList.cpp \
    QGenieAvailableNetworkItem.cpp \
    QGenieRefreshVisibleNetworkThread.cpp \
    QGenieRefreshWChannelViewThread.cpp \
    QGenieConnectToWlanThread.cpp \
    QProgressIndicator.cpp \
    QGenieProgressPanel.cpp \
    QGenieRefreshWirelessStateThread.cpp \
    QGenieWirelessTabWidget.cpp \
    QGenieWlanKeyPersistent.cpp \
    QGenieDialogContainerFrame.cpp \
    QGenieDialogInnerWidget.cpp \
    QGenieWirelessChannelLegendWidget.cpp \
    QGenieWiFiProfileWidget.cpp \
    QGenieWiFiCreateProfileWidget.cpp \
    QGenieWiFiProfileManagerWidget.cpp \
    QGenieWiFiConnectWidget.cpp \
    QGenieConfirmMessageBox.cpp \
    scalabledialogframe.cpp \
    geniepopupdialog.cpp \
    QGenieProfileListItemWidget.cpp \
    QGenieWiFiUtilities.cpp \
    QGenieListWidget.cpp

win32:SOURCES += QGenieWzcMonitorThread.cpp \
    QGenieWiFiProfileManagerController.cpp

macx:SOURCES += QGeniePrivilegeProcess.cpp \
    QGenieWiFiUtilityControllerForMacx.cpp


HEADERS += GeniePlugin_Wireless.h \
    QGenieXLabelPanel.h \
    QGenieWirelessCommon.h \
    QGenieWirelessChannelView.h \
    QGenieVisibleNetworkWidget.h \
    QGenieInputPasswordWidget.h \
    QGenieHistogramWidget.h \
    QGenieCurChannelWidget.h \
    QGenieColumnWidget.h \
    QGenieColumnUnit.h \
    QGenieColumnPanel.h \
    QGenieColumnLabel.h \
    QGenieAvailableNetworkList.h \
    QGenieAvailableNetworkItem.h \
    QGenieRefreshVisibleNetworkThread.h \
    QGenieRefreshWChannelViewThread.h \
    QGenieConnectToWlanThread.h \
    QProgressIndicator.h \
    QGenieProgressPanel.h \
    QGenieRefreshWirelessStateThread.h \
    QGenieWirelessTabWidget.h \
    QGenieWlanKeyPersistent.h \
    QGenieDialogInnerWidget.h \
    QGenieDialogContainerFrame.h \
    QGenieWirelessChannelLegendWidget.h \
    WifiChannelData.h \
    QGenieWiFiProfileWidget.h \
    QGenieWiFiCreateProfileWidget.h \
    QGenieWiFiProfileManagerWidget.h \
    QGenieWiFiConnectWidget.h \
    QGenieConfirmMessageBox.h \
    geniepopupdialog.h \
    scalabledialogframe.h \
    QGenieProfileListItemWidget.h \
    QGenieWiFiUtilities.h \
    QGenieListWidget.h

win32:HEADERS += QGenieWzcMonitorThread.h \
    QGenieWiFiProfileManagerController.h

macx:HEADERS +=  QGeniePrivilegeProcess.h \
    QGenieWiFiUtilityControllerForMacx.h


INCLUDEPATH += ../../include/genie2_interface \
    ../../include/genie2_routeranim \
    ../../include/genie2_netgear \
    .

win32:INCLUDEPATH += ../DiagnosePlugin
macx:INCLUDEPATH += ../

include (../../include/genie2_interface/genie2_interface.pri)
include (../../include/genie2_netgear/genie2_netgear.pri)

#win32:
RESOURCES += QGenieWireless.qrc
#macx:RESOURCES += QGenieWireless_Mac.qrc
macx:LIBS += -framework \
    Security # for QGeniePrivilegeProcess
FORMS += \
    QGenieWiFiProfileWidget.ui \
    QGenieWiFiCreateProfileWidget.ui \
    QGenieProfileListItemWidget.ui
