QT -= gui

TARGET = qtmips_machine
CONFIG += c++11

TEMPLATE = lib

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++0x

DEFINES += QTMIPS_MACHINE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    qtmipsmachine.cpp \
    qtmipsexception.cpp \
    core.cpp \
    memory.cpp \
    instruction.cpp \
    registers.cpp \
    programloader.cpp \
    programmemory.cpp \
    instructions/arithmetic.cpp \
    instructions/loadstore.cpp \
    instructions/shift.cpp \
    instructions/nop.cpp \
    instructions/jumpbranch.cpp \
    cache.cpp

HEADERS += \
    qtmipsmachine.h \
    qtmipsexception.h \
    core.h \
    memory.h \
    instruction.h \
    registers.h \
    programloader.h \
    programmemory.h \
    instructions/arithmetic.h \
    instructions/loadstore.h \
    instructions/shift.h \
    instructions/nop.h \
    instructions/jumpbranch.h \
    cache.h
