#-------------------------------------------------
#
# Project created by QtCreator 2012-12-06T17:54:29
#
#-------------------------------------------------

QT       += testlib

QT       += gui core

TARGET = tst_IntegralImageTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_IntegralImageTest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


#Linkage stuff for linking against "Core"
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Core/release/ -lCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Core/debug/ -lCore
else:unix: LIBS += -L$$OUT_PWD/../Core/ -lCore

INCLUDEPATH += $$PWD/../Core
DEPENDPATH += $$PWD/../Core

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Core/release/Core.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Core/debug/Core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Core/libCore.a

RESOURCES += \
    TestResources.qrc
