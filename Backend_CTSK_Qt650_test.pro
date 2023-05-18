#-------------------------------------------------
#
# Project created by QtCreator 2016-07-24T08:04:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Backend_CTSK_Qt650_test
TEMPLATE = app
CONFIG += console

include(./backendLib.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    WorkThread.cpp \
    CTouchPanelView.cpp \
    ctrldialog.cpp \
    backend_postp.c \
    hid.c \
    backend_draw_thread.cpp


HEADERS  += mainwindow.h \
    WorkThread.h \
    CTouchPanelView.h \
    backend.h \
    ctrldialog.h \
    backend_postp.h \
    hidapi.h \
    backend_draw_thread.h

FORMS    += mainwindow.ui \
    ctrldialog.ui \
    help.ui

win32 {
LIBS += -lhid -lsetupapi
LIBS += "c:/windows/system32/winusb.dll"
#LIBS += "./libbackend/libbackend_qtlib_GN65_221206A.a"

}

#LIBS += "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.17763.0/um/x64/SetupAPI.lib" \
#    "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.17763.0/um/x64/WinMM.Lib"
