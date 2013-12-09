# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core gui
TARGET = GeniePlugin_LibPlugin
GENIE_RELEASE {
    TEMPLATE = lib
    CONFIG += plugin
    DEFINES += GENIE_RELEASE
} else {
    TEMPLATE =app
    SOURCES += main.cpp
	DESTDIR = ../../../trunk/debug
}

LIBS += -L../ -lslib

INCLUDEPATH += ../slib/inc
HEADERS += \
    testplugin.h \
    QMyWidget.h

SOURCES += \
    testplugin.cpp \
    QMyWidget.cpp




RESOURCES += \
    rc/rc.qrc

FORMS += \
    QMyWidget.ui
