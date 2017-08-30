QT += core
QT -= gui

TARGET = qtmips_cli
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

LIBS += -L$$OUT_PWD/../qtmips_machine/ -lqtmips_machine
INCLUDEPATH += $$PWD/../qtmips_machine
DEPENDPATH += $$PWD/../qtmips_machine
QMAKE_CXXFLAGS += -std=c++0x

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
        machineapp.cpp

HEADERS += \
        machineapp.h
