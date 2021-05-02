project(gui
        VERSION 0.7.5
        DESCRIPTION "Graphical UI for the simulator")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(gui_SOURCES
        aboutdialog.cpp
        cachedock.cpp
        cacheview.cpp
        cop0dock.cpp
        coreview/deprecated/connection.cpp
        coreview/deprecated/instructionview.cpp
        coreview/deprecated/logicblock.cpp
        coreview/deprecated/memory.cpp
        coreview/deprecated/minimux.cpp
        coreview/deprecated/multiplexer.cpp
        coreview/deprecated/multitext.cpp
        coreview/deprecated/programcounter.cpp
        coreview/deprecated/registers.cpp
        coreview/deprecated/value.cpp
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
        coreview/components/numeric_value.cpp
        )
set(gui_HEADERS
        aboutdialog.h
        cachedock.h
        cacheview.h
        cop0dock.h
#        coreview/deprecated/connection.h
#        coreview/deprecated/coreview_colors.h
#        coreview/deprecated/instructionview.h
#        coreview/deprecated/logicblock.h
#        coreview/deprecated/memory.h
#        coreview/deprecated/minimux.h
#        coreview/deprecated/multiplexer.h
#        coreview/deprecated/multitext.h
#        coreview/deprecated/programcounter.h
#        coreview/deprecated/registers.h
#        coreview/deprecated/value.h
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
        coreview/components/numeric_value.h
        coreview/data.h
        )
set(gui_UI
        gotosymboldialog.ui
        NewDialog.ui
        peripheralsview.ui
        MainWindow.ui
        NewDialogCache.ui
        )
set(gui_RESOURCES
        icons.qrc
        samples.qrc
        coreview/schemas/schemas.qrc
        )


if ("${WASM}")
    message(STATUS "gui :: Including WASM only files.")
    list(APPEND gui_SOURCES qhtml5file_html5.cpp)
    list(APPEND gui_HEADERS qhtml5file.h)
endif ()


add_executable(gui
        ${gui_SOURCES}
        ${gui_HEADERS}
        ${gui_UI}
        ${gui_RESOURCES})
target_compile_definitions(gui
        PRIVATE
        APP_ORGANIZATION=\"CTU\"
        APP_ORGANIZATION_DOMAIN=\"https://fel.cvut.cz/\"
        APP_NAME=\"QtRVSim\"
        APP_VERSION=\"${PROJECT_VERSION}\"
        ENV_CONFIG_FILE_NAME=\"QTRVSIM_CONFIG_FILE\")
target_include_directories(gui PUBLIC . coreview)
target_link_libraries(gui
        PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui
        PRIVATE machine os_emulation assembler svgscene)

if(${Qt5PrintSupport_FOUND})
    target_link_libraries(gui PRIVATE Qt5::PrintSupport)
    target_compile_definitions(gui PRIVATE WITH_PRINTING=1)
endif()
