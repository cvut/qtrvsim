cmake_minimum_required(VERSION 3.10)

set(ELF_SOURCES
    elf.cc
    to_string.cc
)

# mmap_loader requires POSIX APIs not available on WASM
# QtRvSim uses custom QFileLoader instead
if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
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

# Export include directory
target_include_directories(elf++ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set C++11 standard
set_target_properties(elf++ PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
