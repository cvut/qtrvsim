cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10)

project(QtRVSim
        LANGUAGES C CXX
        VERSION 0.8.0
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

set(COPYRIGHT "Copyright (c) 2017-2019 ${KAREL_KOCI}, \
Copyright (c) 2019-${YEAR} ${PAVEL_PISA}, \
Copyright (c) 2020-${YEAR} ${JAKUB_DUPAK}, \
Copyright (c) 2020-${YEAR} ${MAX_HOLLMANN}")

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


# =============================================================================
# Generated variables
# =============================================================================

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	set(WASM true)
else()
	set(WASM false)
endif()
set(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
set(C_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

# I don't want to relly on the assumption, that this file is invoked as root
# project. Therefore I propagate the information to all subprojects
# MAIN_PROJECT_*. Lowercase and uppercase are used for executable names and
# C defines, respectively.
set(MAIN_PROJECT_NAME "${PROJECT_NAME}")
set(MAIN_PROJECT_VERSION "${PROJECT_VERSION}")
set(MAIN_PROJECT_ORGANIZATION "FEE CTU")
set(MAIN_PROJECT_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
string(TOLOWER "${PROJECT_NAME}" MAIN_PROJECT_NAME_LOWER)
string(TOUPPER "${PROJECT_NAME}" MAIN_PROJECT_NAME_UPPER)

# =============================================================================
# CMake config and tools
# =============================================================================

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(CMAKE_VERSION VERSION_LESS "3.7.0")
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include(cmake/BuildType.cmake)

# =============================================================================
# Build options
# - common to all subdirs
# =============================================================================

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT "${SANITIZERS}" MATCHES "none" AND NOT "${WASM}")
	set(CMAKE_C_FLAGS_DEBUG
	    "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=${SANITIZERS} -g -g3 -ggdb")
	set(CMAKE_CXX_FLAGS_DEBUG
	    "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=${SANITIZERS} -g -g3 -ggdb")
	set(CMAKE_LINKER_FLAGS_DEBUG
	    "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=${SANITIZERS}")
endif()

include_directories("src" "src/machine")

## ============================================================================
## Warning level
## ============================================================================

if(MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		# This is currently a wont-fix and it will be OK in cpp20.
		add_compile_options(-Wno-c99-designator)
	endif()
endif()

# =============================================================================
# Dependencies
# =============================================================================

if("${WASM}")
	message(STATUS "WASM build detected")

	message(STATUS "Enabled WASM exception handling")
	add_link_options("SHELL:-s DISABLE_EXCEPTION_CATCHING=0")
	# Extra options for WASM linking
	add_link_options("SHELL:-s FETCH=1")
	add_link_options("SHELL:-s WASM=1")
	add_link_options("SHELL:-s FULL_ES2=1")
	add_link_options("SHELL:-s FULL_ES3=1")
	add_link_options("SHELL:-s USE_WEBGL2=1")
	add_link_options("SHELL:-s ALLOW_MEMORY_GROWTH=1")
	add_link_options("SHELL:-s EXTRA_EXPORTED_RUNTIME_METHODS=[\"UTF16ToString\",\"stringToUTF16\"]")
	add_link_options("--bind")

	add_compile_definitions(QT_NO_DEBUG_OUTPUT=1)
	message(STATUS "Debug output disabled")

else()
	# Not available for WASM
	enable_testing()

	if(NOT "${FORCE_ELFLIB_STATIC}")
		find_package(LibElf)
		if("${LibElf_FOUND}")
			# Turn non-cmake library into a cmake target
			add_library(libelf INTERFACE)
			target_link_libraries(libelf INTERFACE ${LIBELF_LIBRARY})
			target_include_directories(libelf INTERFACE ${LIBELF_INCLUDE})
			message(STATUS "Using system libelf")
		endif()
	endif()
endif()

if("${WASM}" OR "${FORCE_ELFLIB_STATIC}" OR NOT "${LibElf_FOUND}")
	message(STATUS "Using local libelf fallback.")
	add_subdirectory("external/libelf")
endif()

find_package(Qt5
             REQUIRED COMPONENTS Core Widgets Gui Test
             OPTIONAL_COMPONENTS PrintSupport)

message(STATUS "Qt5 version: ${Qt5Core_VERSION}")
message(STATUS "Qt5 print support: ${Qt5PrintSupport_FOUND}")

add_subdirectory("external/svgscene")

# =============================================================================
# Sources
# =============================================================================

add_subdirectory("src/common")
add_subdirectory("src/machine")
add_subdirectory("src/assembler")
add_subdirectory("src/os_emulation")
add_subdirectory("src/gui")
if(NOT "${WASM}")
	add_subdirectory("src/cli")
endif()

# =============================================================================
# Installation
# =============================================================================

# Prior to CMake version 3.13, installation must be performed in the subdirectory,
# there the target was created. Therefore executable installation is to be found
# in corresponding CMakeLists.txt.

configure_file(data/gui.desktop.in
               "${EXECUTABLE_OUTPUT_PATH}/${MAIN_PROJECT_NAME_LOWER}.desktop")

install(FILES "data/icons/gui.svg"
        DESTINATION "share/icons/hicolor/scalable/apps"
        RENAME "${MAIN_PROJECT_NAME_LOWER}_gui.svg")
install(FILES "data/icons/48x48/gui.png"
        DESTINATION "share/icons/hicolor/48x48/apps"
        RENAME "${MAIN_PROJECT_NAME_LOWER}_gui.png")
install(FILES "${EXECUTABLE_OUTPUT_PATH}/${MAIN_PROJECT_NAME_LOWER}.desktop"
        DESTINATION share/applications)

# =============================================================================
# Packages
# =============================================================================

if("${DEV_MODE}")
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
	set(PACKAGE_LICENCE "${LICENCE}")

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
endif()