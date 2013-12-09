QT       += core gui network


INCLUDEPATH += $$PWD $$PWD/src



include (plc_appengine/plc_appengine.pri)


FORMS += \
    $$PWD/ui/PowerlineWirelessSet.ui \
    $$PWD/ui/PowerlineSecurity_PWD.ui \
    $$PWD/ui/PowerlineSecurity.ui \
    $$PWD/ui/PowerlinePrompt.ui \
    $$PWD/ui/PowerlineDeviceName.ui \
    $$PWD/ui/Powerline_MessageBox.ui \
    $$PWD/ui/Powerline_DetailDlg.ui \
    $$PWD/ui/powerlinepage.ui \
    $$PWD/ui/Powerline_RateShowDlg.ui \
    #$$PWD/ui/Powerline_HubDlg_Detail.ui
    powerline/ui/PowerlineTimeLineBox.ui

HEADERS += \
    $$PWD/src/Singleton.h \
    $$PWD/src/PowerlineWirelessSet.h \
    $$PWD/src/PowerlineView.h \
    $$PWD/src/PowerlineSecurity_PWD.h \
    $$PWD/src/PowerlineSecurity.h \
    $$PWD/src/PowerlinePrompt.h \
    $$PWD/src/PowerlineNode.h \
    $$PWD/src/PowerlineMapView.h \
    $$PWD/src/PowerlineHandler.h \
    $$PWD/src/PowerlineEdge.h \
    $$PWD/src/PowerlineDeviceName.h \
    $$PWD/src/PowerlineDefine.h \
    $$PWD/src/Powerline_MessageBox.h \
    $$PWD/src/Powerline_mapuitilities.h \
    $$PWD/src/Powerline_DetailDlg.h \
    $$PWD/src/PLDevice.h \
    $$PWD/src/DialogMgr.h \
    $$PWD/src/powerlinepage.h \
    $$PWD/src/waitpanel.h \
    $$PWD/src/Powerline_RateShowDlg.h \
    $$PWD/src/Powerline_HubMapView.h \
    #$$PWD/src/Powerline_HubDlg_Detail.h
    $$PWD/src/PowerlineTimeLineBox.h

SOURCES += \
    $$PWD/src/PowerlineWirelessSet.cpp \
    $$PWD/src/PowerlineView.cpp \
    $$PWD/src/PowerlineSecurity_PWD.cpp \
    $$PWD/src/PowerlineSecurity.cpp \
    $$PWD/src/PowerlinePrompt.cpp \
    $$PWD/src/PowerlineNode.cpp \
    $$PWD/src/PowerlineMapView.cpp \
    $$PWD/src/PowerlineHandler.cpp \
    $$PWD/src/PowerlineEdge.cpp \
    $$PWD/src/PowerlineDeviceName.cpp \
    $$PWD/src/Powerline_MessageBox.cpp \
    $$PWD/src/Powerline_mapuitilities.cpp \
    $$PWD/src/Powerline_DetailDlg.cpp \
    $$PWD/src/PLDevice.cpp \
    $$PWD/src/DialogMgr.cpp \
    $$PWD/src/powerlinepage.cpp \
    $$PWD/src/waitpanel.cpp \
    $$PWD/src/Powerline_RateShowDlg.cpp \
    $$PWD/src/Powerline_HubMapView.cpp \
    #$$PWD/src/Powerline_HubDlg_Detail.cpp
    $$PWD/src/PowerlineTimeLineBox.cpp

RESOURCES += \
    $$PWD/res/Powerline.qrc

include($$PWD/smartwifiwizard/smartwifiwizard.pri)
