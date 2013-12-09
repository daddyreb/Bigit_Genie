# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core \  
	widgets \
    gui
TARGET = GeniePlugin_About
DESTDIR = ../../debug
TEMPLATE = lib
CONFIG += plugin

SOURCES += GeniePlugin_About.cpp \
    GenieAboutForm.cpp
HEADERS += GeniePlugin_About.h \
    GenieAboutForm.h
INCLUDEPATH += ..\genie_module\src\inc\genie_interface
    ./inc/genie_interface
FORMS += GenieAboutForm.ui
