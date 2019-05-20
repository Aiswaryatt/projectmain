#-------------------------------------------------
#
# Project created by QtCreator 2019-05-04T14:02:28
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GeneralObjectDetection
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    updatepatches.cpp

HEADERS  += mainwindow.h \
    updatepatches.h

FORMS    += mainwindow.ui

INCLUDEPATH += "E:\opencv\install\include"

Release
{
LIBS += -LE:\opencv\lib    \
-llibopencv_world320 \
-llibopencv_tracking320
}

Debug
{
LIBS += -LE:\opencv\lib    \
-llibopencv_world320d \
-llibopencv_tracking320d
}
