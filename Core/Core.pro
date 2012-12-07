#-------------------------------------------------
#
# Project created by QtCreator 2012-12-06T17:52:38
#
#-------------------------------------------------

QT       += core gui

TARGET = Core
TEMPLATE = lib
CONFIG += staticlib

SOURCES += IntegralImage.cpp

HEADERS += IntegralImage.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
