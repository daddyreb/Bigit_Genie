# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core \
    gui \
    xml \
    network
TARGET = InnerPlugin_TrafficMeter
TEMPLATE = lib
CONFIG += plugin
#GENIE_RELEASE:
#else:DESTDIR = E:\svnHDM\Genie\1.1\plugins\build\debug\main_frame\bin
SOURCES += src/GeniePluginTest.cpp \
    src/TestForm.cpp \
    src/mainwindow.cpp \
    soap/qtsoap.cpp \
    soap/netgearsoapschedule.cpp \
    soap/netgearsoapapi.cpp \
    src/messagebox.cpp \
    src/QProgressIndicator.cpp \
    src/trafficstatistics.cpp \
    src/trafficsetting.cpp \
    src/soapproxy.cpp \
    src/common.cpp \
    src/QGenieMsgDialog.cpp
HEADERS += src/GeniePluginTest.h \
    src/TestForm.h \
    inc/genie_interface/IUiManager.h \
    inc/genie_interface/ITrayManager.h \
    inc/genie_interface/IToyManager.h \
    inc/genie_interface/ISoapManager.h \
    inc/genie_interface/IPluginInf.h \
    inc/genie_interface/IManagerHandler.h \
    inc/genie_interface/IInfoManager.h \
    inc/genie_interface/IClickManager.h \
    src/mainwindow.h \
    inc/genie_interface/ISettings.h \
    inc/genie_interface/info_defs.h \
    inc/genie_interface/ILogManager.h \
    inc/genie_interface/ILanguageManager.h \
    inc/genie_interface/date.h \
    inc/genie_interface/cmd_defs.h \
    inc/genie_interface/bbt_paras.h \
    inc/genie_interface/setting_def.h \
    inc/plugin_ui_def/plugin_ui_def.h \
    soap/qtsoap.h \
    soap/netgearsoapschedule.h \
    soap/NetgearSoapAPI.h \
    inc/multilanguage_def/LanguageMainFrame.h \
    inc/multilanguage_def/LanguageTrafficMeter.h \
    src/messagebox.h \
    src/QProgressIndicator.h \
    src/trafficstatistics.h \
    src/trafficsetting.h \
    src/common.h \
    src/soapproxy.h \
    version.h \
    src/QGenieMsgDialog.h
INCLUDEPATH += ../../src/inc \
    ./inc/genie_interface \
	./soap \
	./src
FORMS += ui/TestForm.ui \
    ui/trafficsetting.ui \
    ui/trafficstatistics.ui \
    ui/QGenieMsgDialog.ui
