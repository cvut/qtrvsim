QT -= gui

TARGET = qtmips_machine
CONFIG += c++14
CONFIG(debug, debug|release) {
    CONFIG += sanitizer sanitize_address
}

TEMPLATE = lib
CONFIG += staticlib

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS_DEBUG += -ggdb
QMAKE_CXXFLAGS_DEBUG += -Wno-c99-designator
QMAKE_CXXFLAGS_DEBUG += -Wextra -Wnull-dereference -Wdouble-promotion -Wformat=2
QMAKE_CXXFLAGS_RELEASE += -Wno-c99-designator

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
    cache.cpp \
    alu.cpp \
    machineconfig.cpp \
    physaddrspace.cpp \
    peripheral.cpp \
    serialport.cpp \
    peripspiled.cpp \
    lcddisplay.cpp \
    symboltable.cpp \
    cop0state.cpp \
    memory/cache/cache_policy.cpp

HEADERS += \
    qtmipsmachine.h \
    qtmipsexception.h \
    core.h \
    memory.h \
    instruction.h \
    registers.h \
    programloader.h \
    cache.h \
    alu.h \
    machineconfig.h \
    utils.h \
    machinedefs.h \
    programloader.h \
    physaddrspace.h \
    peripheral.h \
    serialport.h \
    peripspiled.h \
    lcddisplay.h \
    symboltable.h \
    cop0state.h \
    memory/cache/cache_policy.h \
    memory/cache/cache_types.h
