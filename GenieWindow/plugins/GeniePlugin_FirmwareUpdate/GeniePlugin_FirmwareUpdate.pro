# -------------------------------------------------
# Project created by QtCreator 2010-06-11T10:32:59
# -------------------------------------------------
QT += core \
    gui \
    network
#TARGET = GeniePlugin_FirmwareUpdate
TARGET = InnerPlugin_FirmwareUpdate
TEMPLATE = lib
CONFIG += plugin

# DESTDIR = ../../debug
GENIE_RELEASE:
#else:DESTDIR = D:\Works\NewWorks\main_frame\bin
else:DESTDIR = D:\Works\svnHDM\Genie\1.1\plugins\build\versions\2.2.12\bin

SOURCES += genieplugin_firmwareupdate.cpp \
    introwizard.cpp \
    firmwareupdate.cpp \
    finishwizard.cpp \
    downloadwizard.cpp \
    checkwizard.cpp \
    wizardpage.cpp \
    downloadfirmware.cpp \
    checkfirmware.cpp \
    QProgressIndicator.cpp \
    completewizard.cpp \
    route_animation/QGenieTopoNodePaintStrategy.cpp
HEADERS += genieplugin_firmwareupdate.h \
    firmwareupdate.h \
    finishwizard.h \
    downloadwizard.h \
    checkwizard.h \
    wizardpage.h \
    introwizard.h \
    downloadfirmware.h \
    checkfirmware.h \
    inc/multilanguage_def/LanguageFirmwareUpdate.h \
    QProgressIndicator.h \
    inc/genie_interface/setting_def.h \
    inc/genie_interface/IUiManager.h \
    inc/genie_interface/ITrayManager.h \
    inc/genie_interface/IToyManager.h \
    inc/genie_interface/ISoapManager.h \
    inc/genie_interface/IPluginInf.h \
    inc/genie_interface/info_defs.h \
    inc/genie_interface/IManagerHandler.h \
    inc/genie_interface/ILogManager.h \
    inc/genie_interface/ILanguageManager.h \
    inc/genie_interface/IInfoManager.h \
    inc/genie_interface/IClickManager.h \
    inc/genie_interface/cmd_defs.h \
    inc/genie_interface/bbt_paras.h \
    inc/genie_interface/date.h \
    inc/multilanguage_def/LanguageFirmwareUpdate.h \
    completewizard.h \
    route_animation/QGenieTopoNodePaintStrategy.h
INCLUDEPATH += . \
    ./inc/genie_interface \
    ./inc/plugin_ui_def \
    ./inc/multilanguage_def
RESOURCES += 
