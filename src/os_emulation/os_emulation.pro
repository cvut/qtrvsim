QT -= gui

TARGET = os_emulation
CONFIG += c++14
CONFIG(debug, debug|release) {
    CONFIG += sanitizer sanitize_address
}

TEMPLATE = lib
CONFIG += staticlib

LIBS += -L$$OUT_PWD/../machine/ -lmachine

INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../machine
DEPENDPATH += $$PWD/../machine

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS_DEBUG += -ggdb
QMAKE_CXXFLAGS_DEBUG += -Wextra -Wnull-dereference -Wdouble-promotion -Wformat=2
QMAKE_CXXFLAGS_DEBUG += -Wno-c99-designator
QMAKE_CXXFLAGS_RELEASE += -Wno-c99-designator

DEFINES += OSEMU_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    ossyscall.cpp

HEADERS += \
    ossyscall.h \
    syscall_nr.h \
    target_errno.h
