# -------------------------------------------------
# Project created by QtCreator 2010-05-25T14:37:57
# -------------------------------------------------
QT += core \
    gui \
    network \
    widgets
TARGET = ../../../bin/GeniePlugin_NetworkProblem
TEMPLATE = lib
CONFIG += plugin
SOURCES += src/GeniePluginTest.cpp \
    src/support.cpp \
    src/QProgressIndicator.cpp \
    src/profile.cpp \
    src/ping.cpp \
    src/mainwindow.cpp \
    src/flex.cpp \
    src/faq.cpp \
    src/dnslookup.cpp \
    src/traceroute.cpp \
    src/tools.cpp \
    src/line.cpp \
    src/twidget.cpp \
    src/pushbutton.cpp \
    src/label.cpp
HEADERS += src/GeniePluginTest.h \
    src/support.h \
    src/QProgressIndicator.h \
    src/profile.h \
    src/ping.h \
    src/mainwindow.h \
    src/flex.h \
    src/faq.h \
    src/dnslookup.h \
    src/traceroute.h \
    src/tools.h \
    src/line.h \
    src/twidget.h \
    src/pushbutton.h \
    src/label.h
INCLUDEPATH += .\
			  src\
			 ../genie_module\src\inc\genie_interface\
			 ../genie_module\src

LIBS += -L"../../bin" \
					 -lDragonNetTool
RESOURCES += Images.qrc
include (../../include/genie2_interface/genie2_interface.pri)
