#-------------------------------------------------
#
# Project created by QtCreator 2012-05-24T17:00:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


INCLUDEPATH += $$PWD $$PWD/other

SOURCES += $$PWD/wirelesssettingpannel.cpp \
    $$PWD/wirelesssettingreadonlypannel.cpp \
    $$PWD/wirelesssettingcopyroutersettingpage.cpp \
    $$PWD/enterwirelesssettingpage.cpp \
    $$PWD/smartwirelesssettingwizard.cpp \
    $$PWD/searchrouterpane.cpp \
    $$PWD/wirelesssettingoperatepane.cpp \
    $$PWD/advancewirelesssettingpane.cpp \
    $$PWD/other/progressindicator.cpp

HEADERS  += $$PWD/wirelesssettingpannel.h \
    $$PWD/wirelesssettingreadonlypannel.h \
    $$PWD/wirelesssettingcopyroutersettingpage.h \
    $$PWD/enterwirelesssettingpage.h \
    $$PWD/smartwirelesssettingwizard.h \
    $$PWD/searchrouterpane.h \
    $$PWD/wirelesssettingoperatepane.h \
    $$PWD/advancewirelesssettingpane.h \
    $$PWD/other/progressindicator.h \
    src/smartwifiwizard/other/language_i18n.h

FORMS    += $$PWD/wirelesssettingpannel.ui \
    $$PWD/wirelesssettingreadonlypannel.ui \
    $$PWD/wirelesssettingcopyroutersettingpage.ui \
    $$PWD/enterwirelesssettingpage.ui \
    $$PWD/smartwirelesssettingwizard.ui \
    $$PWD/searchrouterpane.ui \
    $$PWD/wirelesssettingoperatepane.ui \
    $$PWD/advancewirelesssettingpane.ui

RESOURCES += \
    $$PWD/res/sww_res.qrc
