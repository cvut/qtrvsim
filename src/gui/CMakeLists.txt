project(gui
        LANGUAGES C CXX
        VERSION ${MAIN_PROJECT_VERSION}
        DESCRIPTION "Graphical UI for the simulator")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(gui_SOURCES
        dialogs/about/aboutdialog.cpp
        windows/cache/cachedock.cpp
        windows/cache/cacheview.cpp
        windows/csr/csrdock.cpp
        windows/coreview/scene.cpp
        extprocess.cpp
        fontsize.cpp
        dialogs/gotosymbol/gotosymboldialog.cpp
        graphicsview.cpp
        windows/memory/memorydock.cpp
        windows/memory/memorymodel.cpp
        windows/memory/memorytableview.cpp
        windows/messages/messagesdock.cpp
        windows/messages/messagesmodel.cpp
        windows/messages/messagesview.cpp
        dialogs/new/newdialog.cpp
        ui/hexlineedit.cpp
        windows/editor/highlighterasm.cpp
        windows/editor/highlighterc.cpp
        windows/editor/linenumberarea.cpp
        windows/editor/editordock.cpp
        windows/editor/editortab.cpp
        hinttabledelegate.cpp
        windows/lcd/lcddisplaydock.cpp
        windows/lcd/lcddisplayview.cpp
        main.cpp
        mainwindow/mainwindow.cpp
        windows/peripherals/peripheralsdock.cpp
        windows/peripherals/peripheralsview.cpp
        windows/program/programdock.cpp
        windows/program/programmodel.cpp
        windows/program/programtableview.cpp
        windows/registers/registersdock.cpp
        dialogs/savechanged/savechangeddialog.cpp
        windows/editor/srceditor.cpp
        statictable.cpp
        windows/terminal/terminaldock.cpp
        textsignalaction.cpp
        windows/coreview/components/value_handlers.cpp
        windows/coreview/components/cache.cpp
        widgets/hidingtabwidget.cpp
        windows/predictor/predictor_btb_dock.cpp
        windows/predictor/predictor_bht_dock.cpp
        windows/predictor/predictor_info_dock.cpp
        )
set(gui_HEADERS
        dialogs/about/aboutdialog.h
        windows/cache/cachedock.h
        windows/cache/cacheview.h
        windows/csr/csrdock.h
        windows/coreview/scene.h
        extprocess.h
        fontsize.h
        dialogs/gotosymbol/gotosymboldialog.h
        graphicsview.h
        windows/memory/memorydock.h
        windows/memory/memorymodel.h
        windows/memory/memorytableview.h
        windows/messages/messagesdock.h
        windows/messages/messagesmodel.h
        windows/messages/messagesview.h
        dialogs/new/newdialog.h
        ui/hexlineedit.h
        windows/editor/highlighterasm.h
        windows/editor/highlighterc.h
        windows/editor/linenumberarea.h
        windows/editor/editordock.h
        hinttabledelegate.h
        windows/lcd/lcddisplaydock.h
        windows/lcd/lcddisplayview.h
        mainwindow/mainwindow.h
        windows/peripherals/peripheralsdock.h
        windows/peripherals/peripheralsview.h
        windows/program/programdock.h
        windows/program/programmodel.h
        windows/program/programtableview.h
        windows/registers/registersdock.h
        dialogs/savechanged/savechangeddialog.h
        windows/editor/srceditor.h
        statictable.h
        windows/terminal/terminaldock.h
        textsignalaction.h
        windows/coreview/components/value_handlers.h
        windows/coreview/data.h
        windows/coreview/components/cache.h
        helper/async_modal.h
        widgets/hidingtabwidget.h
        windows/predictor/predictor_btb_dock.h
        windows/predictor/predictor_bht_dock.h
        windows/predictor/predictor_info_dock.h
        )
set(gui_UI
        dialogs/gotosymbol/gotosymboldialog.ui
        dialogs/new/NewDialog.ui
        windows/peripherals/peripheralsview.ui
        mainwindow/MainWindow.ui
        dialogs/new/NewDialogCache.ui
        )
set(gui_RESOURCES
        resources/icons/icons.qrc
        resources/samples/samples.qrc
        windows/coreview/schemas/schemas.qrc
        )


if ("${WASM}")
    message(STATUS "gui :: Including WASM only files.")
    list(APPEND gui_SOURCES qhtml5file_html5.cpp)
    list(APPEND gui_HEADERS qhtml5file.h)
endif ()

# MACOS
set(ICON_NAME gui)
set(ICON_PATH ${CMAKE_SOURCE_DIR}/data/icons/macos/${ICON_NAME}.icns)
# END MACOS

add_executable(gui
        ${ICON_PATH}
        ${gui_SOURCES}
        ${gui_HEADERS}
        ${gui_UI}
        ${gui_RESOURCES})
target_include_directories(gui PUBLIC . windows/coreview)
target_link_libraries(gui
        PRIVATE ${QtLib}::Core ${QtLib}::Widgets ${QtLib}::Gui
        PRIVATE machine os_emulation assembler svgscene)
target_compile_definitions(gui
        PRIVATE
        APP_ORGANIZATION=\"${MAIN_PROJECT_ORGANIZATION}\"
        APP_ORGANIZATION_DOMAIN=\"${MAIN_PROJECT_HOMEPAGE_URL}\"
        APP_GIT=\"${MAIN_PROJECT_HOMEPAGE_URL}\"
        APP_NAME=\"${MAIN_PROJECT_NAME}\"
        APP_VERSION=\"${MAIN_PROJECT_VERSION}\"
        ENV_CONFIG_FILE_NAME=\"${MAIN_PROJECT_NAME_UPPER}_CONFIG_FILE\")
set_target_properties(gui PROPERTIES
        OUTPUT_NAME "${MAIN_PROJECT_NAME_LOWER}_${PROJECT_NAME}")

if (${${QtLib}PrintSupport_FOUND} AND NOT ${WASM})
    target_link_libraries(gui PRIVATE ${QtLib}::PrintSupport)
    target_compile_definitions(gui PRIVATE WITH_PRINTING=1)
endif ()

# MACOS
set_property(SOURCE ${ICON_PATH}
        PROPERTY MACOSX_PACKAGE_LOCATION Resources)
set_target_properties(gui PROPERTIES
        MACOSX_BUNDLE true
        MACOSX_BUNDLE_GUI_IDENTIFIER cz.cvut.fel.${MAIN_PROJECT_ORGANIZATION}.gui
        MACOSX_BUNDLE_BUNDLE_NAME ${MAIN_PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION "${MAIN_PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${MAIN_PROJECT_VERSION}"
        MACOSX_BUNDLE_ICONFILE ${ICON_NAME}
        )
# END MACOS

# =============================================================================
# Installation
# =============================================================================

# Prior to CMake version 3.13, installation must be performed in the subdirectory,
# there the target was created. Therefore executable installation is to be found
# in corresponding CMakeLists.txt.

install(TARGETS gui
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION ${EXECUTABLE_OUTPUT_PATH}
        )

