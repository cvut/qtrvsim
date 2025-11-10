cmake_minimum_required(VERSION 3.10)

# Find Python3 for generating to_string.cc
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Generate to_string.cc from data.hh using CMake script (portable across platforms)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/to_string.cc
    COMMAND ${CMAKE_COMMAND} 
        -DPython3_EXECUTABLE=${Python3_EXECUTABLE}
        -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/generate_to_string.cmake
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data.hh 
            ${CMAKE_CURRENT_SOURCE_DIR}/enum-print.py
            ${CMAKE_CURRENT_SOURCE_DIR}/generate_to_string.cmake
    COMMENT "Generating to_string.cc from data.hh"
    VERBATIM
)

set(ELF_SOURCES
    elf.cc
    ${CMAKE_CURRENT_BINARY_DIR}/to_string.cc
)

# mmap_loader requires POSIX APIs (sys/mman.h, unistd.h) not available on Windows or WASM
# QtRvSim uses custom QFileLoader instead
if (NOT WIN32 AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    list(APPEND ELF_SOURCES mmap_loader.cc)
endif()

set(ELF_HEADERS
    elf++.hh
    data.hh
    common.hh
    to_hex.hh
)

add_library(elf++ STATIC ${ELF_SOURCES} ${ELF_HEADERS})

# Disable warnings for this external library
target_compile_options(elf++ PRIVATE -w)

# Export include directories (both source and binary for generated files)
target_include_directories(elf++ PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Set C++11 standard
set_target_properties(elf++ PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
