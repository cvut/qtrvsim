QT += core gui widgets
qtHaveModule(printsupport): QT += printsupport
qtHaveModule(printsupport): DEFINES += WITH_PRINTING=1

TARGET = gui
CONFIG += c++14
CONFIG(debug, debug|release) {
    CONFIG += sanitizer sanitize_address
}

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS_SUBDIR = release
else:win32:CONFIG(debug, debug|release): LIBS_SUBDIR = debug
else:unix: LIBS_SUBDIR = .

LIBS += -L$$OUT_PWD/../os_emulation/$${LIBS_SUBDIR}  -los_emulation
LIBS += -L$$OUT_PWD/../machine/$${LIBS_SUBDIR} -lmachine -lelf
LIBS += -L$$OUT_PWD/../assembler/$${LIBS_SUBDIR} -lassembler -lelf

PRE_TARGETDEPS += $$OUT_PWD/../os_emulation/$${LIBS_SUBDIR}/libos_emulation.a
PRE_TARGETDEPS += $$OUT_PWD/../machine/$${LIBS_SUBDIR}/libmachine.a
PRE_TARGETDEPS += $$OUT_PWD/../assembler/$${LIBS_SUBDIR}/libassembler.a

DOLAR=$

unix: LIBS += \
        -Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\' \
        # --enable-new-dtags \

INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../machine $$PWD/../os_emulation $$PWD/../assembler
DEPENDPATH += $$PWD/../machine $$PWD/../os_emulation $$PWD/../assembler

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS_DEBUG += -ggdb
QMAKE_CXXFLAGS_DEBUG += -Wno-c99-designator
QMAKE_CXXFLAGS_DEBUG += -Wextra -Wnull-dereference -Wdouble-promotion -Wformat=2
QMAKE_CXXFLAGS_RELEASE += -Wno-c99-designator

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
    lcddisplaydock.cpp \
    lcddisplayview.cpp \
    coreview/multitext.cpp \
    fontsize.cpp \
    gotosymboldialog.cpp \
    cop0dock.cpp \
    hinttabledelegate.cpp \
    coreview/minimux.cpp \
    srceditor.cpp \
    highlighterasm.cpp \
    highlighterc.cpp \
    messagesdock.cpp \
    messagesmodel.cpp \
    messagesview.cpp \
    extprocess.cpp \
    savechangeddialog.cpp \
    textsignalaction.cpp

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
    lcddisplaydock.h \
    lcddisplayview.h \
    coreview/multitext.h \
    fontsize.h \
    gotosymboldialog.h \
    cop0dock.h \
    hinttabledelegate.h \
    coreview/minimux.h \
    srceditor.h \
    highlighterasm.h \
    highlighterc.h \
    messagesdock.h \
    messagesmodel.h \
    messagesview.h \
    extprocess.h \
    savechangeddialog.h \
    textsignalaction.h

wassembler: SOURCES += \
    qhtml5file_html5.cpp

wassembler: HEADERS += \
    qhtml5file.h

FORMS += \
        NewDialog.ui \
        NewDialogCache.ui \
        MainWindow.ui \
    peripheralsview.ui \
    gotosymboldialog.ui

RESOURCES += \
        icons.qrc \
        samples.qrc

# ICON is config specific to macOS
# see https://doc.qt.io/qt-5/appicon.html#setting-the-application-icon-on-macos
# see data/icons/macos/README.md
ICON = icons/gui.icns
