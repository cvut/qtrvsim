QT += core gui widgets

TARGET = qtmips_cli
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS_SUBDIR = release
else:win32:CONFIG(debug, debug|release): LIBS_SUBDIR = debug
else:unix: LIBS_SUBDIR = .

LIBS += -L$$OUT_PWD/../qtmips_machine/$${LIBS_SUBDIR} -lqtmips_machine -lelf
LIBS += -L$$OUT_PWD/../qtmips_asm/$${LIBS_SUBDIR} -lqtmips_asm -lelf

PRE_TARGETDEPS += $$OUT_PWD/../qtmips_machine/$${LIBS_SUBDIR}/libqtmips_machine.a
PRE_TARGETDEPS += $$OUT_PWD/../qtmips_asm/$${LIBS_SUBDIR}/libqtmips_asm.a

DOLAR=$

unix: LIBS += \
        -Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\' \
        # --enable-new-dtags \

INCLUDEPATH += $$PWD/../qtmips_machine $$PWD/../qtmips_asm
DEPENDPATH += $$PWD/../qtmips_machine $$PWD/../qtmips_asm
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS_DEBUG += -ggdb

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        tracer.cpp \
        reporter.cpp

HEADERS += \
        tracer.h \
        reporter.h
