#-------------------------------------------------
#
# Project created by QtCreator 2013-04-02T20:47:13
#
#-------------------------------------------------

QT  += core \
    gui \
    network \
    widgets
CONFIG += uitools
INCLUDEPATH += $$PWD/inc

SOURCES += \
    $$PWD/src/extendersoapapi.cpp \
    $$PWD/src/waittingwidget.cpp \
    $$PWD/src/progresswidget.cpp \
    $$PWD/src/extenderutil.cpp \
    $$PWD/src/extenderutilwifi.cpp \
    $$PWD/src/extendermainwindow.cpp \
    $$PWD/src/extendercontrol.cpp \
    $$PWD/src/myprogressindicator.cpp

HEADERS  += \
    $$PWD/src/extendersoapapi.h \
    $$PWD/src/waittingwidget.h \
    $$PWD/src/progresswidget.h \
    $$PWD/src/extenderutil.h \
    $$PWD/src/extenderutilwifi.h \
    $$PWD/src/extendermainwindow.h \
    $$PWD/src/extendercontrol.h \
    $$PWD/src/myprogressindicator.h \
    $$PWD/inc/WiFiApiInterface.h \
    $$PWD/inc/WiFiApiGlobal.h \
    $$PWD/inc/language_mainwindow.h \
    $$PWD/inc/GetOSVer.h \
    $$PWD/inc/DiagnoseInterface.h

FORMS    += \
    $$PWD/ui/mainwindow.ui

RESOURCES += \
    $$PWD/res/res.qrc \

win32: LIBS += -L"../../../bin" -lDiagnosePlugin
macx: LIBS += -L"../../../bin" -lWiFiApiPlugin
