cmake_minimum_required(VERSION 3.10)

project(QtMips
        VERSION 0.7.5
        DESCRIPTION "MIPS CPU simulator for education purposes.")

# =============================================================================
# Configurable options
# =============================================================================

set(FORCE_ELFLIB_STATIC false CACHE BOOL
        "Use included statically linked libelf even if system one is available")

# =============================================================================
# CMake config and tools
# =============================================================================

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif ()

# =============================================================================
# Build options
# - common to all subdirs
# =============================================================================

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("src" "src/machine")

## ============================================================================
## Warning level
## ============================================================================

if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # This is currently a wontfix and it will be OK in cpp20.
        add_compile_options(-Wno-c99-designator)
    endif ()
endif ()

# =============================================================================
# Dependencies
# =============================================================================

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    ## ========================================================================
    ## WASM
    ## ========================================================================

    set(WASM true) # Simplifies other WASM conditions.
    message("WASM build...")

    # Path to WASM compiled Qt. Pass your value via CLI argument if it differs.
    set(CMAKE_FIND_ROOT_PATH "/opt/qt5-wasm" CACHE STRING "WASM QT DIR")

    # Extra options for WASM linking
    add_link_options("SHELL:-s FETCH=1")
    add_link_options("SHELL:-s WASM=1")
    add_link_options("SHELL:-s FULL_ES2=1")
    add_link_options("SHELL:-s FULL_ES3=1")
    add_link_options("SHELL:-s USE_WEBGL2=1")
    add_link_options("SHELL:-s ALLOW_MEMORY_GROWTH=1")
    add_link_options("SHELL:-s EXTRA_EXPORTED_RUNTIME_METHODS=[\"UTF16ToString\",\"stringToUTF16\"]")
    add_link_options("--bind")
else ()
    set(WASM false)

    # Not available for WASM
    enable_testing()

    if (NOT "${FORCE_ELFLIB_STATIC}")
        find_package(LibElf)
        if ("${LibElf_FOUND}")
            # Turn non-cmake library into a cmake target
            add_library(libelf INTERFACE)
            target_link_libraries(libelf INTERFACE ${LIBELF_LIBRARY})
            target_include_directories(libelf INTERFACE ${LIBELF_INCLUDE})
        endif ()
    endif ()
endif ()

if ("${WASM}" OR "${FORCE_ELFLIB_STATIC}" OR NOT "${LibElf_FOUND}")
    message("Using local libelf fallback.")
    add_subdirectory("external/libelf")
endif ()

find_package(Qt5 COMPONENTS Core Widgets Gui Test REQUIRED)

# =============================================================================
# Sources
# =============================================================================

add_subdirectory("src/machine")
add_subdirectory("src/assembler")
add_subdirectory("src/os_emulation")
add_subdirectory("src/gui")
if (NOT "${WASM}")
    add_subdirectory("src/cli")
endif ()
