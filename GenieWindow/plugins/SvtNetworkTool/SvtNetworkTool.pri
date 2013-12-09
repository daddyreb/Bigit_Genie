
HEADERS +=      $$PWD/SvtNetworkTool.h

INCLUDEPATH +=  $$PWD
win32-g++: {
 INCLUDEPATH += $$PWD/libs/mingw $$PWD/libs/mingw/openssl
}
win32-msvc*: {
 INCLUDEPATH += $$PWD/libs/msvc $$PWD/libs/msvc/openssl
}


LIBS += -L../../bin -lSvtNetworkTool
