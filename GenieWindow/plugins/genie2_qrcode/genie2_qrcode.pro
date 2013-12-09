#-------------------------------------------------
#
# Project created by QtCreator 2012-07-03T10:18:59
#
#-------------------------------------------------

QT       -= gui

TEMPLATE = lib

DEFINES += QRCODE_LIBRARY

SOURCES += QGenieQRCode.cpp \
    split.c \
    rscode.c \
    qrspec.c \
    qrinput.c \
    qrencode.c \
    qrenc.c \
    mqrspec.c \
    mmask.c \
    mask.c \
    bitstream.c

HEADERS += QGenieQRCode.h\
    rscode.h \
    qrspec.h \
    qrinput.h \
    qrencode_inner.h \
    qrencode.h \
    mqrspec.h \
    mmask.h \
    mask.h \
    bitstream.h \
    split.h
TARGET = ../../bin/QRCode
DESTDIR = ../genie2_qrcode
