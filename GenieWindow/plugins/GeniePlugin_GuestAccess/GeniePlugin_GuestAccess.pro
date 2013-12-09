# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core \
    gui \
    xml \
    network
TARGET = InnerPlugin_GuestAccess
GENIE_RELEASE:
else:DESTDIR = E:\Qt(NetGear)\Genie(Demo)\Genie(DLL)\Debug\main_frame\bin
TEMPLATE = lib
CONFIG += plugin
SOURCES += GeniePluginTest.cpp \
    TestForm.cpp \
    mainwindow.cpp \
    netgearsoapschedule.cpp \
    netgearsoapapi.cpp \
    qtsoap.cpp \
    QProgressIndicator.cpp \
    messagebox.cpp \
    dialogbox.cpp
HEADERS += GeniePluginTest.h \
    TestForm.h \
    inc/genie_interface/IUiManager.h \
    inc/genie_interface/ITrayManager.h \
    inc/genie_interface/IToyManager.h \
    inc/genie_interface/ISoapManager.h \
    inc/genie_interface/IPluginInf.h \
    inc/genie_interface/IManagerHandler.h \
    inc/genie_interface/IInfoManager.h \
    inc/genie_interface/IClickManager.h \
    inc/plugin_ui_def/plugin_ui_def.h \
    inc/genie_interface/ISettings.h \
    inc/genie_interface/info_defs.h \
    inc/genie_interface/ILogManager.h \
    inc/genie_interface/ILanguageManager.h \
    inc/genie_interface/date.h \
    inc/genie_interface/cmd_defs.h \
    inc/genie_interface/bbt_paras.h \
    inc/genie_interface/setting_def.h \
    mainwindow.h \
    NetgearSoapAPI.h \
    qtsoap.h \
    netgearsoapschedule.h \
    QProgressIndicator.h \
    messagebox.h \
    inc/multilanguage_def/LanguageMainFrame.h \
    inc/multilanguage_def/LanguageGuestAccess.h \
    dialogbox.h
INCLUDEPATH += ../../src/inc \
    ./inc/genie_interface
FORMS += TestForm.ui
RESOURCES += Images.qrc
