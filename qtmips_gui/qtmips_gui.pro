QT += core gui widgets

TARGET = qtmips_gui
CONFIG += c++11

TEMPLATE = app

LIBS += -L$$OUT_PWD/../qtmips_machine/ -lqtmips_machine
INCLUDEPATH += $$PWD/../qtmips_machine
DEPENDPATH += $$PWD/../qtmips_machine
QMAKE_CXXFLAGS += -std=c++0x

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        newdialog.cpp \
        coreview.cpp \
        registersdock.cpp \
        cachestatistics.cpp \
        cachecontent.cpp \
        coreview/programcounter.cpp \
        coreview/multiplexer.cpp \
        coreview/connection.cpp \
        coreview/latch.cpp \
        coreview/alu.cpp

HEADERS += \
        mainwindow.h \
        newdialog.h \
        coreview.h \
        registersdock.h \
        cachestatistics.h \
        cachecontent.h \
        coreview/programcounter.h \
        coreview/multiplexer.h \
        coreview/connection.h \
        coreview/latch.h \
        coreview/alu.h

FORMS += \
        NewDialog.ui \
        MainWindow.ui \
        CacheContent.ui \
        CacheStatistics.ui \
        programmemory.ui

RESOURCES += \
        icons.qrc
