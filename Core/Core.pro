#-------------------------------------------------
#
# Project created by QtCreator 2012-12-06T17:52:38
#
#-------------------------------------------------

QT       += core gui

TARGET = Core
TEMPLATE = lib
CONFIG += staticlib

SOURCES += IntegralImage.cpp \
    ClassifierChain.cpp \
    Classifier.cpp \
    SimpleClassifier.cpp \
    Feature.cpp \
    TwoRectFeature.cpp \
    ThreeRectFeature.cpp

HEADERS += IntegralImage.h \
    ClassifierChain.h \
    Classifier.h \
    SimpleClassifier.h \
    Feature.h \
    TwoRectFeature.h \
    ThreeRectFeature.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
