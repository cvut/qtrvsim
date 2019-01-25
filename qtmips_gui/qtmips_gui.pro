QT += core gui widgets

TARGET = qtmips_gui
CONFIG += c++11

TEMPLATE = app

LIBS += -L$$OUT_PWD/../qtmips_machine/ -lqtmips_machine

DOLAR=$

unix: LIBS += \
        -Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\' \
        # --enable-new-dtags \

INCLUDEPATH += $$PWD/../qtmips_machine
DEPENDPATH += $$PWD/../qtmips_machine
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS_DEBUG += -ggdb

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        newdialog.cpp \
        coreview.cpp \
        registersdock.cpp \
        programdock.cpp \
        memorydock.cpp \
        memoryview.cpp \
        coreview/programcounter.cpp \
        coreview/multiplexer.cpp \
        coreview/connection.cpp \
        coreview/latch.cpp \
        coreview/alu.cpp \
        coreview/memory.cpp \
        coreview/instructionview.cpp \
        coreview/registers.cpp \
        coreview/adder.cpp \
        coreview/constant.cpp \
        coreview/junction.cpp \
        coreview/logicblock.cpp \
        coreview/and.cpp \
        statictable.cpp \
    cacheview.cpp \
    cachedock.cpp \
    graphicsview.cpp \
    coreview/value.cpp

HEADERS += \
        mainwindow.h \
        newdialog.h \
        coreview.h \
        registersdock.h \
        programdock.h \
        memorydock.h \
        memoryview.h \
        coreview/programcounter.h \
        coreview/multiplexer.h \
        coreview/connection.h \
        coreview/latch.h \
        coreview/alu.h \
        coreview/memory.h \
        coreview/instructionview.h \
        coreview/registers.h \
        coreview/adder.h \
        coreview/constant.h \
        coreview/junction.h \
        coreview/logicblock.h \
        coreview/and.h \
        statictable.h \
    cacheview.h \
    cachedock.h \
    graphicsview.h \
    coreview/value.h

FORMS += \
        NewDialog.ui \
        NewDialogCache.ui \
        MainWindow.ui

RESOURCES += \
        icons.qrc
