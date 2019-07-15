QT -= gui

TARGET = qtmips_asm
CONFIG += c++11

TEMPLATE = lib
CONFIG += staticlib

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS_DEBUG += -ggdb

DEFINES += QTMIPS_MACHINE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    fixmatheval.cpp

HEADERS += \
    fixmatheval.h
