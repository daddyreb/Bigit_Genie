TEMPLATE += fakelib
TOPOLOGY_STATIC_LIBNAME =$$qtLibraryTarget(Topology)
TEST_TARGET=$$qtLibraryTarget(TestNetscan)
TEMPLATE -= fakelib
TARGET = $$TEST_TARGET

CONFIG += console
#TEMPLATE = lib
#CONFIG += staticlib
DESTDIR =../testnetscan
INCLUDEPATH +=    ../../inc
QT += network xml
win32:LIBS += -lws2_32
CONFIG-=app_bundle

LIBS += -L../../Build/Win32 -l$$TOPOLOGY_STATIC_LIBNAME

SOURCES += \
    main.cpp
