QT += testlib
QT -= gui

TARGET = tst_machine
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS_SUBDIR = release
else:win32:CONFIG(debug, debug|release): LIBS_SUBDIR = debug
else:unix: LIBS_SUBDIR = .

LIBS += -L$$OUT_PWD/../$${LIBS_SUBDIR} -lqtmips_machine -lelf

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
