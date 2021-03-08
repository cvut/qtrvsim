cmake_minimum_required(VERSION 3.10)

project(QtMips
        VERSION 0.7.5
        DESCRIPTION "MIPS CPU simulator for education purposes.")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif ()

enable_testing()

find_package(Qt5 COMPONENTS Core Widgets Gui Test REQUIRED)

include_directories("src" "src/machine")

add_subdirectory("src/machine")
add_subdirectory("src/assembler")
add_subdirectory("src/os_emulation")
add_subdirectory("src/gui")
add_subdirectory("src/cli")
