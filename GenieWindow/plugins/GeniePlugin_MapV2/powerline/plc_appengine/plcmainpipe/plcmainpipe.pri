QT += core network xml

INCLUDEPATH += $$PWD $$PWD/plcsoap $$PWD/plcapi $$PWD/include $$PWD/core

SOURCES += \
    $$PWD/plcapi/plcdrivermanager.cpp \
    $$PWD/plcapi/plcdriverexecutive.cpp \
    $$PWD/plcapi/plcdriver.cpp \
    $$PWD/plcapi/helperroutines.cpp \
    $$PWD/plcsoap/soapreturnhandler.cpp \
    $$PWD/plcsoap/smarthandle.cpp \
    $$PWD/plcsoap/networktransactionmanager.cpp \
    $$PWD/plcsoap/networktransaction.cpp \
    $$PWD/plcsoap/networkaccessproxy.cpp \
    $$PWD/plcsoap/netgearsoapapi.cpp \
    $$PWD/core/plcmainpipeplugin.cpp \
    $$PWD/plcapi/mainpipeapiguard.cpp

HEADERS += \
    $$PWD/plcapi/plcdrivermanager.h \
    $$PWD/plcapi/plcdriverinterface.h \
    $$PWD/plcapi/plcdriverexecutive.h \
    $$PWD/plcapi/plcdriver.h \
    $$PWD/plcapi/helperroutines.h \
    $$PWD/plcsoap/soapreturnhandler.h \
    $$PWD/plcsoap/smarthandle.h \
    $$PWD/plcsoap/networktransactionmanager.h \
    $$PWD/plcsoap/networktransaction.h \
    $$PWD/plcsoap/networkaccessproxy.h \
    $$PWD/plcsoap/netgearsoapapi.h \
    $$PWD/core/plcmainpipeplugin.h \
    $$PWD/include/mainpipedefines.h \
    $$PWD/include/mainpipeinterface.h \
    $$PWD/include/mainpipeeventmonitorinterface.h \
    $$PWD/plcapi/mainpipeapiguard.h

win32{
HEADERS += \
    $$PWD/platform/win32/filesystemmonitor.h \
    $$PWD/platform/win32/nethelproutines.h

SOURCES += \
    $$PWD/platform/win32/filesystemmonitor.cpp \
    $$PWD/platform/win32/nethelproutines.cpp

INCLUDEPATH += $$PWD/platform/win32

LIBS += -lIphlpapi
}

DEFINES += DUMP_LAYER2_API
