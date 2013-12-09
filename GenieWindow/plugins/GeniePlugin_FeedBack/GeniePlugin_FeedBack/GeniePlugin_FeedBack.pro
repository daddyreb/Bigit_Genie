TEMPLATE = lib
CONFIG += plugin

QT += network
TARGET = GeniePlugin_FeedBack
HEADERS += qgeniefeedbackplugin.h \
    workpanel.h \
    inc/genie_interface/IPluginInf.h \
    inc/multilanguage_def/LanguageFeedback.h \
    inc/genie_interface/IManagerHandler.h \
    inc/genie_interface/bbt_paras.h \
    inc/genie_interface/IUiManager.h \
    QProgressIndicator.h \
    selfdef.h \
    qLineEditPlaceTxt.h \
    qTextEditPlaceTxt.h \
    version.h \
    SendMailThread.h
SOURCES += qgeniefeedbackplugin.cpp \
    workpanel.cpp \
    QProgressIndicator.cpp \
    qLineEditPlaceTxt.cpp \
    qTextEditPlaceTxt.cpp \
    SendMailThread.cpp

INCLUDEPATH += . \
    ../../../../language_workspace\macro_defines

include (../../../../trunk/src/genie2_interface/genie2_interface.pri)
