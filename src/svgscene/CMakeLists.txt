project(svgscene)
cmake_minimum_required(VERSION 3.10)

find_package(Qt5
        REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(svgscene)
target_sources(svgscene
        PRIVATE
        necrolog/include/necrolog.h
        necrolog/libnecrolog/necrolog.cpp
        necrolog/libnecrolog/necrolog.h
        necrolog/libnecrolog/necrologglobal.h
        src/log.h
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        src/svggraphicsview.cpp
        src/svggraphicsview.h
        src/svghandler.cpp
        src/svghandler.h
        src/types.h
        src/types.cpp
        src/simpletextitem.h
        src/simpletextitem.cpp
        src/groupitem.h
        src/groupitem.cpp
        )
target_include_directories(svgscene
        PRIVATE . src necrolog)
target_link_libraries(svgscene
        PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)