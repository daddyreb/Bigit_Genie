TEMPLATE = lib
CONFIG += plugin
QT       += core gui xml network widgets
HEADERS += qgenieparentalcontrol.h \
    pcpform.h \
    labelbtn.h \
     ../../genie_module/src/inc/genie_interface/IPluginInf.h	\
    ../../genie_module/src/inc/genie_interface/ITrayManager.h \
    ../../genie_module/src/inc/genie_interface/IToyManager.h \
    ../../genie_module/src/inc/genie_interface/ISoapManager.h \
    ../../genie_module/src/inc/genie_interface/IPluginInf.h \
    ../../genie_module/src/inc/genie_interface/IManagerHandler.h \
    ../../genie_module/src/inc/genie_interface/ILanguageManager.h \
    ../../genie_module/src/inc/genie_interface/IInfoManager.h \
    ../../genie_module/src/inc/genie_interface/IClickManager.h \
    ../../genie_module/src/inc/genie_interface/IUiManager.h \
    ../../genie_module/src/inc/multilanguage_def/LanguageParentalControl.h \
    ../../genie_module/src/inc/genie_interface/bbt_paras.h \
    ../selefdef.h \
    version.h \
    QGenieDialogInnerWidget.h \
    QGenieDialogContainerFrame.h \
    QGeniePluginCableWidget.h
SOURCES += qgenieparentalcontrol.cpp \
    pcpform.cpp \
    labelbtn.cpp \
    QGeniePluginCableWidget.cpp \
    QGenieDialogInnerWidget.cpp \
    QGenieDialogContainerFrame.cpp
RESOURCES += Resources.qrc
GENIE_RELEASE:
else:DESTDIR = D:\plugin\bin
TARGET = GeniePlugin_ParentalControl
INCLUDEPATH += ../../genie_module/src/inc/genie_interface \
	../../genie_module/src \
	../../genie_module/src/inc/plugin_ui_def\
    ./inc/genie_interface \
    ./inc/plugin_ui_def \
    ../../genie_module/src/inc/multilanguage_def \
    ./
