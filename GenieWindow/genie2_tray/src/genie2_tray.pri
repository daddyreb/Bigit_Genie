QT *= xml
QT *= network
CONFIG *= uitools
HEADERS +=$$PWD/genie2_tray.h \
    $$PWD/QGenieTrayManager.h \
    $$PWD/QGenieTrayIco_Out.h \
    $$PWD/QGenieTrayIco_In.h \
    $$PWD/QGenieSelfTrayMessage.h \
    $$PWD/IGenieTrayIco.h \
    $$PWD/trayico_out_cmds.h \
    $$PWD/QGenieTray_Communication_Verify.h

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/QGenieTrayManager.cpp \
    $$PWD/QGenieTrayIco_Out.cpp \
    $$PWD/QGenieTrayIco_In.cpp \
    $$PWD/QGenieSelfTrayMessage.cpp \
    $$PWD/QGenieTray_Communication_Verify.cpp

macx:FORMS += $$PWD/QGenieSelfTrayMessage_Mac.ui
else:FORMS += $$PWD/QGenieSelfTrayMessage_Windows.ui


