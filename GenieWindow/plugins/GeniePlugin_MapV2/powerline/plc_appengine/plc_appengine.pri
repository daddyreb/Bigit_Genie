QT += core network

INCLUDEPATH += \
    $$PWD \
    $$PWD/core/include \
    $$PWD/core \
    $$PWD/transactions/include \
    $$PWD/transactions \
#    $$PWD/inc/include

HEADERS += \
    $$PWD/core/plctransactionmanager.h \
    $$PWD/core/plctopologydata.h \
    $$PWD/core/plcdevicescanner.h \
    $$PWD/core/plcdevice.h \
    $$PWD/core/plcappengineexecutive.h \
    $$PWD/transactions/plc_tsetdevicename.h \
    $$PWD/transactions/plc_tsetdeviceled.h \
    $$PWD/transactions/plc_tsetdevicesecuritykey.h \
    $$PWD/transactions/plc_tdeviceresetdefault.h \
    $$PWD/transactions/include/transactions.h \
    $$PWD/core/pointertransfer.h \
    $$PWD/core/plcappengine.h \
    $$PWD/transactions/plc_tbatchsetdevicessecuritykey.h \
    $$PWD/transactions/plc_tsetdevicewlaninfo.h \
    $$PWD/transactions/plc_tresynchscan.h \
    $$PWD/transactions/plc_tcheckfirmwareupgrade.h \
    $$PWD/transactions/plc_tgetdevicewlaninfo.h \
    $$PWD/core/plcdevicesoapthread.h

SOURCES += \
    $$PWD/core/plctransactionmanager.cpp \
    $$PWD/core/plctopologydata.cpp \
    $$PWD/core/plcdevicescanner.cpp \
    $$PWD/core/plcdevice.cpp \
    $$PWD/core/plcappengineexecutive.cpp \
    $$PWD/transactions/plc_tsetdevicename.cpp \
    $$PWD/transactions/plc_tsetdeviceled.cpp \
    $$PWD/transactions/plc_tsetdevicesecuritykey.cpp \
    $$PWD/transactions/plc_tdeviceresetdefault.cpp \
    $$PWD/core/plcappengine.cpp \
    $$PWD/transactions/plc_tbatchsetdevicessecuritykey.cpp \
    $$PWD/transactions/plc_tsetdevicewlaninfo.cpp \
    $$PWD/transactions/plc_tresynchscan.cpp \
    $$PWD/transactions/plc_tcheckfirmwareupgrade.cpp \
    $$PWD/transactions/plc_tgetdevicewlaninfo.cpp \
    $$PWD/core/plcdevicesoapthread.cpp

include(plcmainpipe/plcmainpipe.pri)
