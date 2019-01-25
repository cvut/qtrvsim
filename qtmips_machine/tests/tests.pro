QT += testlib
QT -= gui

TARGET = tst_machine
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

LIBS += -L$$OUT_PWD/../ -lqtmips_machine

DOLAR=$

unix: LIBS += \
        -Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\' \
        # --enable-new-dtags \

INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..
QMAKE_CXXFLAGS += -std=c++0x

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += tst_machine.cpp \
    testmemory.cpp \
    testregisters.cpp \
    testprogramloader.cpp \
    testinstruction.cpp \
    testalu.cpp \
    testcore.cpp \
    testcache.cpp

HEADERS += tst_machine.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"
