project(qtmips_gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(qtmips_gui_SOURCES
        aboutdialog.cpp
        cachedock.cpp
        cacheview.cpp
        cop0dock.cpp
        coreview/adder.cpp
        coreview/alu.cpp
        coreview/and.cpp
        coreview/connection.cpp
        coreview/constant.cpp
        coreview/instructionview.cpp
        coreview/junction.cpp
        coreview/latch.cpp
        coreview/logicblock.cpp
        coreview/memory.cpp
        coreview/minimux.cpp
        coreview/multiplexer.cpp
        coreview/multitext.cpp
        coreview/programcounter.cpp
        coreview/registers.cpp
        coreview/value.cpp
        coreview.cpp
        extprocess.cpp
        fontsize.cpp
        gotosymboldialog.cpp
        graphicsview.cpp
        memorydock.cpp
        memorymodel.cpp
        memorytableview.cpp
        messagesdock.cpp
        messagesmodel.cpp
        messagesview.cpp
        newdialog.cpp
        hexlineedit.cpp
        highlighterasm.cpp
        highlighterc.cpp
        hinttabledelegate.cpp
        lcddisplaydock.cpp
        lcddisplayview.cpp
        main.cpp
        mainwindow.cpp
        peripheralsdock.cpp
        peripheralsview.cpp
        programdock.cpp
        programmodel.cpp
        programtableview.cpp
        registersdock.cpp
        savechangeddialog.cpp
        srceditor.cpp
        statictable.cpp
        terminaldock.cpp
        textsignalaction.cpp
        )
set(qtmips_gui_HEADERS
        aboutdialog.h
        cachedock.h
        cacheview.h
        cop0dock.h
        coreview/adder.h
        coreview/alu.h
        coreview/and.h
        coreview/connection.h
        coreview/constant.h
        coreview/coreview_colors.h
        coreview/instructionview.h
        coreview/junction.h
        coreview/latch.h
        coreview/logicblock.h
        coreview/memory.h
        coreview/minimux.h
        coreview/multiplexer.h
        coreview/multitext.h
        coreview/programcounter.h
        coreview/registers.h
        coreview/value.h
        coreview.h
        extprocess.h
        fontsize.h
        gotosymboldialog.h
        graphicsview.h
        memorydock.h
        memorymodel.h
        memorytableview.h
        messagesdock.h
        messagesmodel.h
        messagesview.h
        newdialog.h
        hexlineedit.h
        highlighterasm.h
        highlighterc.h
        hinttabledelegate.h
        lcddisplaydock.h
        lcddisplayview.h
        mainwindow.h
        peripheralsdock.h
        peripheralsview.h
        programdock.h
        programmodel.h
        programtableview.h
        registersdock.h
        savechangeddialog.h
        srceditor.h
        statictable.h
        terminaldock.h
        textsignalaction.h
        )
set(qtmips_gui_UI
        gotosymboldialog.ui
        NewDialog.ui
        peripheralsview.ui
        MainWindow.ui
        NewDialogCache.ui
        )
set(qtmips_gui_RESOURCES
        icons.qrc
        samples.qrc
        )

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    message("qtmips_gui :: Including WASM only files.")
    list(APPEND qtmips_gui_SOURCES qhtml5file_html5.cpp)
    list(APPEND qtmips_gui_HEADERS qhtml5file.h)
endif ()


add_executable(qtmips_gui
        ${qtmips_gui_SOURCES}
        ${qtmips_gui_HEADERS}
        ${qtmips_gui_UI}
        ${qtmips_gui_RESOURCES})

target_include_directories(qtmips_gui PUBLIC . coreview)
target_link_libraries(qtmips_gui
        PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui
        PRIVATE qtmips_machine qtmips_osemu qtmips_asm)
