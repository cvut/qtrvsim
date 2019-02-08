QT -= gui

TARGET = qtmips_osemu
CONFIG += c++11

TEMPLATE = lib

LIBS += -L$$OUT_PWD/../qtmips_machine/ -lqtmips_machine

INCLUDEPATH += $$PWD/../qtmips_machine $$PWD/../qtmips_osemu
DEPENDPATH += $$PWD/../qtmips_machine

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS_DEBUG += -ggdb

DEFINES += QTMIPS_OSEMU_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    ossyscall.cpp

HEADERS += \
    ossyscall.h
