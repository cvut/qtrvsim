QT += testlib
QT -= gui

TARGET = tst_machine
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++14
CONFIG += sanitizer sanitize_address

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS_SUBDIR = release
else:win32:CONFIG(debug, debug|release): LIBS_SUBDIR = debug
else:unix: LIBS_SUBDIR = .

LIBS += -L$$OUT_PWD/../$${LIBS_SUBDIR} -lqtmips_machine -lelf

PRE_TARGETDEPS += $$OUT_PWD/../$${LIBS_SUBDIR}/libqtmips_machine.a

DOLAR=$

unix: LIBS += \
        -Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\' \
        # --enable-new-dtags \

INCLUDEPATH += $$PWD/../..
INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/..
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS_DEBUG += -Wno-c99-designator
QMAKE_CXXFLAGS_RELEASE += -Wno-c99-designator

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
