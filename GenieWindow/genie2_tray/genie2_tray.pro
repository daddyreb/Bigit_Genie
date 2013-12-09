TARGET=../../bin/genie2_tray
QT           += network \
                widgets

SOURCES += \
    main.cpp \
    QGenieTrayService.cpp

HEADERS += \
    QGenieTrayService.h
include (./src/genie2_tray.pri)
#include (../../../../src/genie2_interface/genie2_interface.pri)

INCLUDEPATH += ../include/genie2_interface
#DESTDIR = ../../../../debug
#DEFINES += DBG_MODE

