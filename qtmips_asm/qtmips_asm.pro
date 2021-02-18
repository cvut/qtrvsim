QT -= gui

TARGET = qtmips_asm
CONFIG += c++14
CONFIG(debug, debug|release) {
    CONFIG += sanitizer sanitize_address
}

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../qtmips_machine
DEPENDPATH += $$PWD/../qtmips_machine
QMAKE_CXXFLAGS_DEBUG += -fsanitize=address -fno-omit-frame-pointer
QMAKE_LFLAGS_DEBUG += -fsanitize=address

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS_DEBUG += -ggdb
QMAKE_CXXFLAGS_DEBUG += -Wextra -Wnull-dereference -Wdouble-promotion -Wformat=2
QMAKE_CXXFLAGS_DEBUG += -Wno-c99-designator
QMAKE_CXXFLAGS_RELEASE += -Wno-c99-designator

DEFINES += QTMIPS_MACHINE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    fixmatheval.cpp \
    simpleasm.cpp

HEADERS += \
    fixmatheval.h \
    messagetype.h \
    simpleasm.h
