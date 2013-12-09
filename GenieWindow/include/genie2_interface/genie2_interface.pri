HEADERS +=$$PWD/genie2_interface.h \
    $$PWD/IPluginInf2.h \
    $$PWD/interface2_cmds.h \
    $$PWD/interface2_macros.h \
    $$PWD/QGenieIface2ShareObject.h \
    $$PWD/IHandlerInf.h \
    $$PWD/interface2_settings.h \
    $$PWD/inner_macros.h \
    $$PWD/genie2_plugincommunication.h
INCLUDEPATH += $$PWD

SOURCES += $$PWD/QGenieIface2ShareObject.cpp
GENIE_RELEASE:defines += GENIE_RELEASE
