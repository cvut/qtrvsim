QT += core gui widgets

TARGET = cli
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++14
CONFIG(debug, debug|release) {
    CONFIG += sanitizer sanitize_address
}

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS_SUBDIR = release
else:win32:CONFIG(debug, debug|release): LIBS_SUBDIR = debug
else:unix: LIBS_SUBDIR = .

LIBS += -L$$OUT_PWD/../machine/$${LIBS_SUBDIR} -lmachine -lelf
LIBS += -L$$OUT_PWD/../assembler/$${LIBS_SUBDIR} -lassembler -lelf

PRE_TARGETDEPS += $$OUT_PWD/../machine/$${LIBS_SUBDIR}/libmachine.a
PRE_TARGETDEPS += $$OUT_PWD/../assembler/$${LIBS_SUBDIR}/libassembler.a

DOLAR=$

unix: LIBS += \
        -Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\' \
        # --enable-new-dtags \

INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../machine $$PWD/../asm
DEPENDPATH += $$PWD/../machine $$PWD/../asm

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS_DEBUG += -ggdb
QMAKE_CXXFLAGS_DEBUG += -Wextra -Wnull-dereference -Wdouble-promotion -Wformat=2
QMAKE_CXXFLAGS_DEBUG += -Wno-c99-designator
QMAKE_CXXFLAGS_RELEASE += -Wno-c99-designator

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        tracer.cpp \
        reporter.cpp \
        msgreport.cpp \
        chariohandler.cpp

HEADERS += \
        tracer.h \
        reporter.h \
        msgreport.h \
        chariohandler.h
