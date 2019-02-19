QT += core gui widgets

TARGET = qtmips_gui
CONFIG += c++11

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS_SUBDIR = release
else:win32:CONFIG(debug, debug|release): LIBS_SUBDIR = debug
else:unix: LIBS_SUBDIR = .

LIBS += -L$$OUT_PWD/../qtmips_osemu/$${LIBS_SUBDIR}  -lqtmips_osemu
LIBS += -L$$OUT_PWD/../qtmips_machine/$${LIBS_SUBDIR} -lqtmips_machine -lelf

DOLAR=$

unix: LIBS += \
        -Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\' \
        # --enable-new-dtags \

INCLUDEPATH += $$PWD/../qtmips_machine $$PWD/../qtmips_osemu
DEPENDPATH += $$PWD/../qtmips_machine $$PWD/../qtmips_osemu
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
    coreview/value.cpp \
    memorymodel.cpp \
    memorytableview.cpp \
    hexlineedit.cpp \
    programmodel.cpp \
    programtableview.cpp \
    aboutdialog.cpp \
    peripheralsdock.cpp \
    terminaldock.cpp \
    peripheralsview.cpp \
    coreview/multitext.cpp \
    fontsize.cpp

HEADERS += \
        mainwindow.h \
        newdialog.h \
        coreview.h \
        registersdock.h \
        programdock.h \
        memorydock.h \
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
    coreview/value.h \
    memorymodel.h \
    memorytableview.h \
    hexlineedit.h \
    programmodel.h \
    programtableview.h \
    aboutdialog.h \
    peripheralsdock.h \
    terminaldock.h \
    peripheralsview.h \
    coreview/multitext.h \
    fontsize.h

FORMS += \
        NewDialog.ui \
        NewDialogCache.ui \
        MainWindow.ui \
    peripheralsview.ui

RESOURCES += \
        icons.qrc
