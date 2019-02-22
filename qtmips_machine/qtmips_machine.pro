QT -= gui

TARGET = qtmips_machine
CONFIG += c++11

TEMPLATE = lib

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS_DEBUG += -ggdb

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
    utils.cpp \
    physaddrspace.cpp \
    peripheral.cpp \
    serialport.cpp \
    peripspiled.cpp \
    symboltable.cpp

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
    physaddrspace.h \
    peripheral.h \
    serialport.h \
    peripspiled.h \
    symboltable.h
