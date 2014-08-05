#-------------------------------------------------
#
# Project created by QtCreator 2014-06-24T11:24:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test-qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp
SOURCES += thread_manualcontrol.cpp
SOURCES += waypointspath_control.cpp
SOURCES += help_window.cpp

HEADERS  += mainwindow.h
HEADERS  += thread_manualcontrol.h
HEADERS += waypointspath_control.h
HEADERS  += help_window.h

FORMS    += mainwindow.ui
FORMS    += help_window.ui
