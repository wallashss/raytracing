#-------------------------------------------------
#
# Project created by QtCreator 2014-06-03T22:49:08
#
#-------------------------------------------------

QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = Raytracing
TEMPLATE = app

INCLUDEPATH += glm

SOURCES += main.cpp\
    raytracing.cpp \
    drawable.cpp \
    light.cpp \
    raytracing_canvas.cpp

HEADERS  += \
    raytracing.h \
    drawable.h \
    light.h \
    raytracing_canvas.h

