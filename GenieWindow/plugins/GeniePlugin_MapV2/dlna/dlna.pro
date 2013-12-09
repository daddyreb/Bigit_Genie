#-------------------------------------------------
#
# Project created by QtCreator 2013-10-14T13:37:44
#
#-------------------------------------------------

QT       -= gui

TARGET = dlna
DESTDIR = ../../../bin
TEMPLATE = lib
CONFIG += staticlib

DEFINES += WIN32 NPT_CONFIG_ENABLE_LOGGING

INCLUDEPATH += ./src \
            ./src/Core \
            ./src/DLNACore \
            ./src/System \
            ./src/System/Win32 \

SOURCES += \
    Src/DLNACore/SVTasks.cpp \
    Src/DLNACore/SVTaskGroup.cpp \
    Src/DLNACore/SVMediaStoreImpl.cpp \
    Src/DLNACore/SVMediaStore.cpp \
    Src/DLNACore/SVFrontEnd.cpp \
    Src/DLNACore/SVDeviceImpl.cpp \
    Src/DLNACore/SVDescPriv.cpp \
    Src/DLNACore/SVDesc.cpp \
    Src/DLNACore/SVControlPoint.cpp \
    Src/DLNACore/SVConnector.cpp \
    Src/DLNACore/DLNAObjectImpl.cpp \
    Src/DLNACore/DLNAObject.cpp \
    Src/DLNACore/DLNALibraryImpl.cpp \
    Src/DLNACore/DLNALibrary.cpp \
    Src/DLNACore/DLNACoreOpImpl.cpp \
    Src/DLNACore/DLNACoreOp.cpp \
    Src/DLNACore/DLNACoreImpl.cpp \
    Src/DLNACore/DLNACore.cpp \
    Src/DLNACore/avcore4.cpp \
    Src/DLNACore/avcore3.cpp \
    Src/DLNACore/avcore2.cpp \
    Src/DLNACore/avcore1.cpp \
    Src/DLNACore/SVUtils.cpp \
    Src/System/Bsd/NptBsdSockets.cpp \
    Src/Core/NptUtils.cpp \
    Src/Core/NptUri.cpp \
    Src/Core/NptTime.cpp \
    Src/Core/NptThreads.cpp \
    Src/Core/NptSystem.cpp \
    Src/Core/NptStrings.cpp \
    Src/Core/NptStreams.cpp \
    Src/Core/NptSockets.cpp \
    Src/Core/NptResults.cpp \
    Src/Core/NptLogging.cpp \
    Src/Core/NptList.cpp \
    Src/Core/NptHttp.cpp \
    Src/Core/NptHash.cpp \
    Src/Core/NptFile.cpp \
    Src/Core/NptDynamicLibraries.cpp \
    Src/Core/NptDebug.cpp \
    Src/Core/NptDataBuffer.cpp \
    Src/Core/NptBufferedStreams.cpp \
    Src/Core/NptXml.cpp \
    Src/System/StdC/NptStdcEnvironment.cpp \
    Src/System/StdC/NptStdcFile.cpp \
    Src/System/Win32/NptWin32Threads.cpp \
    Src/System/Win32/NptWin32System.cpp \
    Src/System/Win32/NptWin32Network.cpp \
    Src/System/Win32/NptWin32File.cpp \
    Src/System/Win32/NptWin32Debug.cpp \
    Src/System/Win32/NptWin32Time.cpp \
    Src/System/WinCE/NptWinceConsole.cpp \
    Src/DLNACore/SVIconsData.cpp \
    Src/System/Posix/NptPosixFile.cpp \
    Src/Core/NptNetwork.cpp

HEADERS += \
    Src/DLNACore/SVTasks.h \
    Src/DLNACore/SVTaskGroup.h \
    Src/DLNACore/SVMediaStoreImpl.h \
    Src/DLNACore/SVMediaStore.h \
    Src/DLNACore/SVFrontEnd.h \
    Src/DLNACore/SVDeviceImpl.h \
    Src/DLNACore/SVDescPriv.h \
    Src/DLNACore/SVDesc.h \
    Src/DLNACore/SVControlPoint.h \
    Src/DLNACore/SVConnector.h \
    Src/DLNACore/DLNAObjectImpl.h \
    Src/DLNACore/DLNAObject.h \
    Src/DLNACore/DLNALibraryImpl.h \
    Src/DLNACore/DLNALibrary.h \
    Src/DLNACore/DLNACoreOpImpl.h \
    Src/DLNACore/DLNACoreOp.h \
    Src/DLNACore/DLNACoreImpl.h \
    Src/DLNACore/DLNACore.h \
    Src/DLNACore/avcore4.h \
    Src/DLNACore/avcore3.h \
    Src/DLNACore/avcore2.h \
    Src/DLNACore/avcore1.h \
    Src/DLNACore/SVUtils.h \
    Src/Core/NptUtils.h \
    Src/Core/NptUri.h \
    Src/Core/NptTypes.h \
    Src/Core/NptTime.h \
    Src/Core/NptThreads.h \
    Src/Core/NptSystem.h \
    Src/Core/NptStrings.h \
    Src/Core/NptStreams.h \
    Src/Core/NptResults.h \
    Src/Core/NptLogging.h \
    Src/Core/NptList.h \
    Src/Core/NptInterfaces.h \
    Src/Core/NptHttp.h \
    Src/Core/NptHash.h \
    Src/Core/NptFile.h \
    Src/Core/NptDynamicLibraries.h \
    Src/Core/NptDynamicCast.h \
    Src/Core/NptDebug.h \
    Src/Core/NptDataBuffer.h \
    Src/Core/NptConstants.h \
    Src/Core/NptConfig.h \
    Src/Core/NptBufferedStreams.h \
    Src/Core/NptArray.h \
    Src/Core/NptXml.h \
    Src/System/Win32/NptWin32Network.h \
    Src/System/Win32/NptWin32Threads.h \
    Src/System/WinCE/NptWinCeUtils.h \
    Src/Core/NptSockets.h \
    Src/Core/NptReferences.h \
    Src/Core/NptMap.h \
    Src/Core/NptNetwork.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
