cmake_minimum_required(VERSION 3.10)
project(svgscene)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
# Normally, we would use variable Qt5 or Qt6 to reference the Qt library. Here we do that through
# this variable based on detected version major of Qt.
set(QtLib "Qt${QT_VERSION_MAJOR}")
find_package(${QtLib}
		REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(svgscene STATIC
		src/svgscene/components/groupitem.cpp
		src/svgscene/components/groupitem.h
		src/svgscene/components/hyperlinkitem.cpp
		src/svgscene/components/hyperlinkitem.h
		src/svgscene/components/simpletextitem.cpp
		src/svgscene/components/simpletextitem.h
		src/svgscene/graphicsview/svggraphicsview.cpp
		src/svgscene/graphicsview/svggraphicsview.h
		src/svgscene/svgdocument.cpp
		src/svgscene/svgdocument.h
		src/svgscene/svggraphicsscene.cpp
		src/svgscene/svggraphicsscene.h
		src/svgscene/svghandler.cpp
		src/svgscene/svghandler.h
		src/svgscene/svgmetadata.cpp
		src/svgscene/svgmetadata.h
		src/svgscene/svgspec.h
		src/svgscene/polyfills/qt5/qwheelevent.h
		src/svgscene/polyfills/qt5/qstringview.h
		src/svgscene/utils/logging.h
		src/svgscene/utils/memory_ownership.h
		)
target_link_libraries(svgscene
		PRIVATE ${QtLib}::Core ${QtLib}::Gui ${QtLib}::Widgets)
target_include_directories(svgscene PUBLIC src PRIVATE src/svgscene)

add_executable(svgscene-example EXCLUDE_FROM_ALL
		src/example/main.cpp
		src/example/mainwindow.cpp
		src/example/mainwindow.h
		src/example/mainwindow.ui
		)
target_link_libraries(svgscene-example
		PRIVATE ${QtLib}::Core ${QtLib}::Gui ${QtLib}::Widgets svgscene)