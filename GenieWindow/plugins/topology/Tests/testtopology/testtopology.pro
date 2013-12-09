TEMPLATE += fakelib
TOPOLOGY_STATIC_LIBNAME =$$qtLibraryTarget(Topology)
TEST_TARGET=$$qtLibraryTarget(TestTopology)
TEMPLATE -= fakelib
TARGET = testtopology#$$TEST_TARGET

CONFIG += app
#TEMPLATE = lib
#CONFIG += staticlib

QT += network xml
DESTDIR =../testtopology
INCLUDEPATH +=    ../../inc

LIBS += -L../../Build/MacOS -l$$TOPOLOGY_STATIC_LIBNAME
win32:LIBS += -lws2_32


SOURCES += \
    main.cpp \
    TestTopologyForm.cpp \
    SVTController.cpp

HEADERS += \
    TestTopologyForm.h \
    SVTController.h

FORMS += \
    TestTopologyForm.ui
