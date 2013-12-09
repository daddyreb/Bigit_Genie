QT          += gui core network xml \
               widgets
TEMPLATE = lib
DEFINES += SVT_NETWORKTOOL_LIBRARY

DEFINES += SM_ONLY_ONE_SESSION
HEADERS +=      $$PWD/SvtNetworkTool.h \
		$$PWD/SvtHttpDownload.h \
                $$PWD/SvtFcml.h \
                $$PWD/SvtFcmlError.h
SOURCES +=      $$PWD/SvtBlockedSocket.cpp \
		$$PWD/SvtNetwork.cpp \
		$$PWD/SvtHttp.cpp \
		$$PWD/SvtSoap.cpp \
		$$PWD/SvtHttpDownload.cpp \
                $$PWD/SvtFcml.cpp
INCLUDEPATH +=  $$PWD


win32-g++: {
 LIBS += -L$$PWD/libs/mingw -lssl -lcrypto
 INCLUDEPATH += $$PWD/libs/mingw $$PWD/libs/mingw/openssl
}
win32-msvc*: {
 LIBS += -L$$PWD/libs/msvc -llibeay32 -lssleay32
 INCLUDEPATH += $$PWD/libs/msvc $$PWD/libs/msvc/openssl
}
win32:LIBS += -lws2_32
macx: LIBS += -lssl -lcrypto

TEMPLATE += fakelib
SVT_NETWORKTOOL_LIBNAME =$$qtLibraryTarget(SvtNetworkTool)
TEMPLATE -= fakelib
TARGET = ../../../bin/$$SVT_NETWORKTOOL_LIBNAME
#DESTDIR = ../SvtNetworkTool

