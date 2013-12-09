#-------------------------------------------------
#
# Project created by QtCreator 2013-05-06T17:04:24
#
#-------------------------------------------------

QT       += widgets network xml

DESTDIR = ../../bin/
TARGET = SvtNetworkTool
TEMPLATE = lib

DEFINES += SVT_NETWORKTOOL_LIBRARY \
			WIN32_LEAN_AND_MEAN

SOURCES += \
    ./SvtSoap.cpp \
    ./SvtNetwork.cpp \
    ./SvtHttpDownload.cpp \
    ./SvtHttp.cpp \
    ./SvtFcml.cpp \
    ./SvtBlockedSocket.cpp

HEADERS +=  ./SvtSoap.h \
    ./SvtNetworkTool.h \
    ./SvtNetwork.h \
    ./SvtHttpDownload.h \
    ./SvtHttp.h \
    ./SvtFcmlError.h \
    ./SvtFcml.h \
    ./SvtBlockedSocket.h

win32-g++: {
 LIBS += -L$$PWD/libs/mingw -lssl -lcrypto
 INCLUDEPATH += $$PWD/libs/mingw $$PWD/libs/mingw/openssl
}
win32-msvc*: {
 LIBS += -L$$PWD/libs/msvc -llibeay32 -lssleay32
 INCLUDEPATH += $$PWD/libs/msvc $$PWD/libs/msvc/openssl
}
win32:LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32
macx: LIBS += -lssl -lcrypto

#TEMPLATE += fakelib
#SVT_NETWORKTOOL_LIBNAME =$$qtLibraryTarget(SvtNetworkTool)
#TEMPLATE -= fakelib

