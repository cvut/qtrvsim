cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10)

project(QtRVSim
        LANGUAGES C CXX
        VERSION 0.9.8
        DESCRIPTION "RISC-V CPU simulator for education purposes")

set(KAREL_KOCI "Karel Koci <cynerd@email.cz>")
set(PAVEL_PISA "Pavel Pisa <pisa@cmp.felk.cvut.cz>")
set(JAKUB_DUPAK "Jakub Dupak <dev@jakubdupak.com>")
set(MAX_HOLLMANN "Max Hollmann <hollmmax@fel.cvut.cz>")

set(PROJECT_HOMEPAGE_URL "https://github.com/cvut/qtrvsim")
set(GENERIC_NAME "RISC-V CPU simulator")
set(LICENCE "GPL-3.0-or-later")
set(LONG_DESCRIPTION
        "RISC-V CPU simulator for education purposes with pipeline and cache visualization.")
string(TIMESTAMP YEAR "%Y")

include(cmake/CopyrightTools.cmake)


copyright(
        "Copyright (c) 2017-2019 ${KAREL_KOCI}"
        "Copyright (c) 2019-${YEAR} ${PAVEL_PISA}"
        "Copyright (c) 2020-${YEAR} ${JAKUB_DUPAK}"
        "Copyright (c) 2020-2021 ${MAX_HOLLMANN}")

include(cmake/GPL-3.0-or-later.cmake)

# =============================================================================
# Configurable options
# =============================================================================

set(DEV_MODE false CACHE BOOL "Enable developer options in this CMake, like packaging.\
    They should be ignored, when user just wants to build this project.")
set(FORCE_ELFLIB_STATIC false CACHE BOOL
        "Use included statically linked libelf even if system one is available.")
set(SANITIZERS "address,undefined" CACHE STRING
        "Runtime sanitizers to use in debug builds.
    Column separated subset of {address, memory, undefined, thread} or none.
    Memory and address cannot be used at the same time.")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/target"
        CACHE STRING "Absolute path to place executables to.")
set(PACKAGE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/pkg"
        CACHE STRING "Absolute path to place generated package files.")
set(FORCE_COLORED_OUTPUT false CACHE BOOL "Always produce ANSI-colored output (GNU/Clang only).")
set(USE_ALTERNATE_LINKER "" CACHE STRING "Use alternate linker. Leave empty for system default; alternatives are 'gold', 'lld', 'bfd', 'mold'")
set(QT_VERSION_MAJOR "auto" CACHE STRING "Qt major version to use. 5|6|auto")

# =============================================================================
# Generated variables
# =============================================================================

if (NOT "${QT_VERSION_MAJOR}" MATCHES "5|6|auto")
    message(FATAL_ERROR "Invalid value for QT_VERSION_MAJOR: ${QT_VERSION_MAJOR} (expected 5, 6 or auto)")
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(WASM true)
else ()
    set(WASM false)
endif ()
set(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
set(C_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})


macro(set_alternate_linker linker)
    find_program(LINKER_EXECUTABLE ld.${USE_ALTERNATE_LINKER} ${USE_ALTERNATE_LINKER})
    if (LINKER_EXECUTABLE)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 12.0.0)
            add_link_options("-ld-path=${USE_ALTERNATE_LINKER}")
        else ()
            add_link_options("-fuse-ld=${USE_ALTERNATE_LINKER}")
        endif ()
    else ()
        set(USE_ALTERNATE_LINKER "" CACHE STRING "Use alternate linker" FORCE)
    endif ()
endmacro()
if (NOT "${USE_ALTERNATE_LINKER}" STREQUAL "")
    set_alternate_linker(${USE_ALTERNATE_LINKER})
endif ()

# I don't want to relly on the assumption, that this file is invoked as root
# project. Therefore I propagate the information to all subprojects
# MAIN_PROJECT_*. Lowercase and uppercase are used for executable names and
# C defines, respectively.
set(MAIN_PROJECT_NAME "${PROJECT_NAME}")
set(MAIN_PROJECT_VERSION "${PROJECT_VERSION}")
set(MAIN_PROJECT_APPID "cz.cvut.edu.comparch.qtrvsim")
set(MAIN_PROJECT_ORGANIZATION "FEE CTU")
set(MAIN_PROJECT_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
string(TOLOWER "${PROJECT_NAME}" MAIN_PROJECT_NAME_LOWER)
string(TOUPPER "${PROJECT_NAME}" MAIN_PROJECT_NAME_UPPER)

# =============================================================================
# CMake config and tools
# =============================================================================

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif ()

include(cmake/BuildType.cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# =============================================================================
# Build options
# - common to all subdirs
# =============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC is not supported, Clang+MSVC currently does not seem to support sanitizers in with debug CRT,
#  MinGW does not support sanitizers at all; all together, just skip sanitizers on Windows and re-investigate
#  in a year or two whether Clang sanitizer support has improved
if (NOT "${SANITIZERS}" MATCHES "none" AND NOT "${WASM}" AND NOT "${WIN32}")
    set(CMAKE_C_FLAGS_DEBUG
            "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=${SANITIZERS} -g -g3 -ggdb")
    set(CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=${SANITIZERS} -g -g3 -ggdb")
    set(CMAKE_LINKER_FLAGS_DEBUG
            "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=${SANITIZERS}")
endif ()

if (NOT "${BUILD_DEBUG}")
    message(STATUS "Debug prints globally suppressed.")
    add_definitions(-DQT_NO_DEBUG_OUTPUT=1)
endif ()

include_directories("src" "src/machine")


if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()

## ============================================================================
## Warning level
## ============================================================================

if (WIN32)
    # otherwise CRT complains that `strerror` is deprecated
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif ()

if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -Werror=switch)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # This is currently a wont-fix and it will be OK in cpp20.
        add_compile_options(-Wno-c99-designator)
    endif ()
endif ()

# =============================================================================
# Dependencies
# =============================================================================

if ("${WASM}")
    message(STATUS "WASM build detected")

    message(STATUS "Enabled WASM exception handling")
    add_compile_options("-fexceptions")
    # Extra options for WASM linking
    add_link_options("-fexceptions")
    add_link_options("-flto")
    # Activate Embind C/C++ bindings
    # https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html
    add_link_options("--bind")
    # Enable C++ exception catching
    # https://emscripten.org/docs/optimizing/Optimizing-Code.html#c-exceptions
    add_link_options("SHELL:-s DISABLE_EXCEPTION_CATCHING=0")
    # Enable Fetch API
    # https://emscripten.org/docs/api_reference/fetch.html
    add_link_options("SHELL:-s FETCH=1")
    add_link_options("SHELL:-s WASM=1")
    # Emulate missing OpenGL ES2/ES3 features
    # https://emscripten.org/docs/porting/multimedia_and_graphics/OpenGL-support.html#opengl-es-2-0-3-0-emulation
    add_link_options("SHELL:-s FULL_ES2=1")
    add_link_options("SHELL:-s FULL_ES3=1")
    # Activate WebGL 2 (in addition to WebGL 1)
    # https://emscripten.org/docs/porting/multimedia_and_graphics/OpenGL-support.html#webgl-friendly-subset-of-opengl-es-2-0-3-0
    add_link_options("SHELL:-s USE_WEBGL2=1")
    # Run static dtors at teardown
    # https://emscripten.org/docs/getting_started/FAQ.html#what-does-exiting-the-runtime-mean-why-don-t-atexit-s-run
    add_link_options("SHELL:-s ALLOW_MEMORY_GROWTH=1")
    # Export UTF16ToString,stringToUTF16
    # Required by https://codereview.qt-project.org/c/qt/qtbase/+/286997 (since Qt 5.14)
    add_link_options("SHELL:-s EXTRA_EXPORTED_RUNTIME_METHODS=[\"UTF16ToString\",\"stringToUTF16\"]")
    # Enable demangling of C++ stack traces
    # https://emscripten.org/docs/porting/Debugging.html
    add_link_options("SHELL:-s DEMANGLE_SUPPORT=1")
    # Run static dtors at teardown
    # https://emscripten.org/docs/getting_started/FAQ.html#what-does-exiting-the-runtime-mean-why-don-t-atexit-s-run
    add_link_options("SHELL:-s EXIT_RUNTIME=1")

    # Debug build
    if ("${BUILD_DEBUG}")
        message(STATUS "Enabled WASM debug")
        add_link_options("SHELL:-s ERROR_ON_WASM_CHANGES_AFTER_LINK")
        add_link_options("SHELL:-s WASM_BIGINT")
        add_link_options("-O1")
    endif ()

    add_definitions(-DQT_NO_DEBUG_OUTPUT=1)
    message(STATUS "Debug output disabled")
else ()
    # Not available for WASM
    enable_testing()

    if (NOT "${FORCE_ELFLIB_STATIC}")
        find_package(LibElf)
        if ("${LibElf_FOUND}")
            include(CheckSymbolExists)
            check_symbol_exists(EM_RISCV "gelf.h" LIBELF_HAS_RISCV)
            if ("${LIBELF_HAS_RISCV}")
                # Turn non-cmake library into a cmake target
                add_library(libelf INTERFACE)
                target_link_libraries(libelf INTERFACE ${LIBELF_LIBRARY})
                target_include_directories(libelf INTERFACE ${LIBELF_INCLUDE_DIR})
                message(STATUS "Using system libelf")
            else ()
                message(STATUS "System libelf does not support RISC-V")
                set(LibElf_FOUND FALSE) # Force fallback
            endif ()
        endif ()
    endif ()
endif ()

if ("${WASM}" OR "${FORCE_ELFLIB_STATIC}" OR NOT "${LibElf_FOUND}")
    message(STATUS "Using local libelf fallback.")
    add_subdirectory("external/libelf")
endif ()

# Detect Qt used qt version
# Based on article https://www.steinzone.de/wordpress/how-to-support-both-qt5-and-qt6-using-cmake/
# Cannot use version-less approach due to Qt 5.9.5 support constraint.

if ("${QT_VERSION_MAJOR}" STREQUAL "auto")
    find_package(QT NAMES Qt5 Qt6 COMPONENTS Core REQUIRED)
endif ()

# Normally, we would use variable Qt5 or Qt6 to reference the Qt library. Here we do that through
# this variable based on detected version major of Qt.
set(QtLib "Qt${QT_VERSION_MAJOR}")
find_package(${QtLib}
        REQUIRED COMPONENTS Core Widgets Gui Test
        OPTIONAL_COMPONENTS PrintSupport)

message(STATUS "${QtLib} version: ${${QtLib}Core_VERSION}")
message(STATUS "${QtLib} print support: ${${QtLib}PrintSupport_FOUND}")

if ("${WASM}")
    string(TIMESTAMP DATE "%Y%m%d")
    file(READ "${${QtLib}Core_DIR}/../../../plugins/platforms/qtloader.js" QTLOADER)
    string(REPLACE "applicationName + \".js\"" "applicationName + \".js?v=${DATE}\"" QTLOADER "${QTLOADER}")
    string(REPLACE "applicationName + \".wasm\"" "applicationName + \".wasm?v=${DATE}\"" QTLOADER "${QTLOADER}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/target/qtloader.js" "${QTLOADER}")
endif ()


# Qt 5.9.5 is the oldest supported
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050905)

add_subdirectory("external/svgscene")

# =============================================================================
# Sources
# =============================================================================

add_subdirectory("src/common")
add_subdirectory("src/machine")
add_subdirectory("src/assembler")
add_subdirectory("src/os_emulation")
add_subdirectory("src/gui")
if (NOT "${WASM}")
    add_subdirectory("src/cli")
    add_custom_target(all_unit_tests
            DEPENDS common_unit_tests machine_unit_tests)
endif ()

# =============================================================================
# Installation
# =============================================================================

# Prior to CMake version 3.13, installation must be performed in the subdirectory,
# there the target was created. Therefore executable installation is to be found
# in corresponding CMakeLists.txt.

if (NOT "${WASM}")
    configure_file(data/gui.desktop.in
            "${EXECUTABLE_OUTPUT_PATH}/${MAIN_PROJECT_NAME_LOWER}.desktop")
    configure_file("data/${MAIN_PROJECT_APPID}.metainfo.xml.in"
            "${EXECUTABLE_OUTPUT_PATH}/${MAIN_PROJECT_APPID}.metainfo.xml")

    install(FILES "data/icons/gui.svg"
            DESTINATION "share/icons/hicolor/scalable/apps"
            RENAME "${MAIN_PROJECT_NAME_LOWER}_gui.svg")
    install(FILES "data/icons/48x48/gui.png"
            DESTINATION "share/icons/hicolor/48x48/apps"
            RENAME "${MAIN_PROJECT_NAME_LOWER}_gui.png")
    install(FILES "${EXECUTABLE_OUTPUT_PATH}/${MAIN_PROJECT_NAME_LOWER}.desktop"
            DESTINATION share/applications)

    install(FILES "${EXECUTABLE_OUTPUT_PATH}/${MAIN_PROJECT_APPID}.metainfo.xml"
            DESTINATION share/metainfo)
endif ()

# =============================================================================
# Packages
# =============================================================================

if ("${DEV_MODE}")
    # The condition prevents execution of this section during regular user installation.
    # It created files useless to normal users and requires additional tools (git, xz).
    message(STATUS "Packaging tools enabled.")

    set(PACKAGE_NAME "${MAIN_PROJECT_NAME_LOWER}")
    set(PACKAGE_VERSION "${PROJECT_VERSION}")
    set(PACKAGE_RELEASE "1")
    set(PACKAGE_SOURCE_ARCHIVE_FILE "${PACKAGE_NAME}_${PACKAGE_VERSION}.orig.tar.xz")
    set(PACKAGE_SOURCE_ARCHIVE_PATH "${PACKAGE_OUTPUT_PATH}/${PACKAGE_SOURCE_ARCHIVE_FILE}")
    set(PACKAGE_TOPLEVEL_DIR "${PACKAGE_NAME}-${PACKAGE_VERSION}")
    set(PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
    set(PACKAGE_LONG_DESCRIPTION "${LONG_DESCRIPTION}")
    set(PACKAGE_MAINTAINER "${JAKUB_DUPAK}")
    set(PACKAGE_URL "${PROJECT_HOMEPAGE_URL}")
    set(PACKAGE_GIT "github.com:cvut/qtrvsim.git")
    set(PACKAGE_LICENCE "${LICENCE} ")

    include(cmake/PackageTools.cmake)

    # Inject up-to-date information into package config files.
    package_config_file(appimage appimage.yml extras/packaging/appimage/appimage.yml.in)
    package_config_file(archlinux PKGBUILD extras/packaging/arch/PKGBUILD.in)
    package_config_file(rpm ${PACKAGE_NAME}.spec extras/packaging/rpm/spec.in)
    # Debian uses whole directory which has to be saved to archive and shipped.
    package_debian_quilt(deb
            ${PACKAGE_NAME}_${PACKAGE_VERSION}-${PACKAGE_RELEASE}.dsc
            extras/packaging/deb/dsc.in
            extras/packaging/deb/debian
            ${PACKAGE_NAME}_${PACKAGE_VERSION}-${PACKAGE_RELEASE}.debian.tar.xz)
    # Creates bunch of files in ${CMAKE_BINARY_DIR}/target/pkg that you can just pass to
    # Open Build Service and it will build all packaging.
    # TODO: Currently changelog is not handled automatically.
    add_custom_target(open_build_service_bundle
            DEPENDS ${PACKAGE_SOURCE_ARCHIVE_FILE} appimage archlinux deb rpm
            WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/pkg)
endif ()

configure_file(src/project_info.h.in ${CMAKE_CURRENT_LIST_DIR}/src/project_info.h @ONLY)
