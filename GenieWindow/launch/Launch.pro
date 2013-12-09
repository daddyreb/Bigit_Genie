# -------------------------------------------------
# Project created by QtCreator 2010-06-12T11:27:54
# -------------------------------------------------
TARGET = ../../bin/NETGEARGenie
TEMPLATE = app

QT     += widgets
CONFIG += debug_and_release
INCLUDEPATH += . \
    ./inc \
    ./inc/genie_interface \
    ./inc/launch_interface \
    ./inc/multilanguage_def \
    ./share \
    ./src \
    ../include/genie2_interface
HEADERS += src/QLauncher.h \ 
    src/QGenieConfigFileMover.h
SOURCES += src/main.cpp \
    src/QLauncher.cpp \ 
    src/QGenieConfigFileMover.cpp
DEFINES += REDIRECT_DIR

win32:RC_FILE = ico.rc

#DESTDIR = ../trunk/debug
