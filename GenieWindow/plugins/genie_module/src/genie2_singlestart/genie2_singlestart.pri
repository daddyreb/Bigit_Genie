
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/QLaunchSemaphore.h \
    $$PWD/QGenieSingleProcessHandler.h \
    $$PWD/genie2_singlestart.h \
    $$PWD/SingleInstance.h \
    $$PWD/QGenieWatchDog.h

SOURCES += \
    $$PWD/QLaunchSemaphore.cpp \
    $$PWD/QGenieSingleProcessHandler.cpp \
    $$PWD/QGenieWatchDog.cpp
macx:SOURCES +=$$PWD/SingleInstance.c
