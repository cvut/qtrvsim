QT += core gui widgets
qtHaveModule(printsupport): QT += printsupport
qtHaveModule(printsupport): DEFINES += QTMIPS_WITH_PRINTING=1

TARGET = qtmips_gui
CONFIG += c++11

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS_SUBDIR = release
else:win32:CONFIG(debug, debug|release): LIBS_SUBDIR = debug
else:unix: LIBS_SUBDIR = .

LIBS += -L$$OUT_PWD/../qtmips_osemu/$${LIBS_SUBDIR}  -lqtmips_osemu
LIBS += -L$$OUT_PWD/../qtmips_machine/$${LIBS_SUBDIR} -lqtmips_machine -lelf
LIBS += -L$$OUT_PWD/../qtmips_asm/$${LIBS_SUBDIR} -lqtmips_asm -lelf

PRE_TARGETDEPS += $$OUT_PWD/../qtmips_osemu/$${LIBS_SUBDIR}/libqtmips_osemu.a
PRE_TARGETDEPS += $$OUT_PWD/../qtmips_machine/$${LIBS_SUBDIR}/libqtmips_machine.a
PRE_TARGETDEPS += $$OUT_PWD/../qtmips_asm/$${LIBS_SUBDIR}/libqtmips_asm.a

DOLAR=$

unix: LIBS += \
        -Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\' \
        # --enable-new-dtags \

INCLUDEPATH += $$PWD/../qtmips_machine $$PWD/../qtmips_osemu $$PWD/../qtmips_asm
DEPENDPATH += $$PWD/../qtmips_machine $$PWD/../qtmips_osemu $$PWD/../qtmips_asm
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
    lcddisplaydock.cpp \
    lcddisplayview.cpp \
    coreview/multitext.cpp \
    fontsize.cpp \
    gotosymboldialog.cpp \
    cop0dock.cpp \
    hinttabledelegate.cpp \
    coreview/minimux.cpp \
    srceditor.cpp \
    highlighter.cpp

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
    highlighter.h

wasm: SOURCES += \
    qhtml5file_html5.cpp

wasm: HEADERS += \
    qhtml5file.h

FORMS += \
        NewDialog.ui \
        NewDialogCache.ui \
        MainWindow.ui \
    peripheralsview.ui \
    gotosymboldialog.ui

RESOURCES += \
        icons.qrc

# ICON is config specific to macOS
# see https://doc.qt.io/qt-5/appicon.html#setting-the-application-icon-on-macos
# see data/icons/macos/README.md
ICON = icons/qtmips_gui.icns
