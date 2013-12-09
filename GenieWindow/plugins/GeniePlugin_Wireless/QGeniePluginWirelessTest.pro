# -------------------------------------------------
# Project created by QtCreator 2010-06-21T11:14:31
# -------------------------------------------------
QT += core \
    gui \
    xml \
    network
TARGET = GeniePluginWirelessTest
TEMPLATE = app
macx:DESTDIR = ../../bin
macx:DEFINES += ENBABLE_UPDATEWIRELESS_REALTIME
#win32:
DEFINES += ENABLE_WIFIPROFILE_MANAGER
CONFIG += plugin
macx:CONFIG += x86 ppc
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
    soap/qtsoap.cpp \
    soap/netgearsoapschedule.cpp \
    soap/netgearsoapapi.cpp \
    QGenieWiFiProfileWidget.cpp \
    QGenieWiFiCreateProfileWidget.cpp \
    QGenieWiFiProfileManagerWidget.cpp \
    QGenieWiFiConnectWidget.cpp \
    QGenieConfirmMessageBox.cpp \
    scalabledialogframe.cpp \
    geniepopupdialog.cpp \
    QGenieProfileListItemWidget.cpp \
    TestWirelessPluginMain.cpp

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
    soap/qtsoap.h \
    soap/netgearsoapschedule.h \
    soap/netgearsoapapi.h \
    WifiChannelData.h \
    QGenieWiFiProfileWidget.h \
    QGenieWiFiCreateProfileWidget.h \
    QGenieWiFiProfileManagerWidget.h \
    QGenieWiFiConnectWidget.h \
    QGenieConfirmMessageBox.h \
    geniepopupdialog.h \
    scalabledialogframe.h \
    QGenieProfileListItemWidget.h

win32:HEADERS += QGenieWzcMonitorThread.h \
    QGenieWiFiProfileManagerController.h

macx:HEADERS +=  QGeniePrivilegeProcess.h \
    QGenieWiFiUtilityControllerForMacx.h


INCLUDEPATH += ../inc/genie_interface \
    ../inc/plugin_ui_def \
    ../inc/multilanguage_def \
    ./soap \
    .
win32:INCLUDEPATH += ../lib/DiagnosePlugin
macx:INCLUDEPATH += ../lib
win32:RESOURCES += QGenieWireless.qrc
macx:RESOURCES += QGenieWireless_Mac.qrc
macx:LIBS += -framework \
    Security # for QGeniePrivilegeProcess
FORMS += \
    QGenieWiFiProfileWidget.ui \
    QGenieWiFiCreateProfileWidget.ui \
    QGenieProfileListItemWidget.ui
