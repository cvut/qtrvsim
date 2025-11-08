cmake_minimum_required(VERSION 3.10)

set(DWARF_SOURCES
    abbrev.cc
    attrs.cc
    cursor.cc
    die.cc
    die_str_map.cc
    dwarf.cc
    elf.cc
    expr.cc
    line.cc
    rangelist.cc
    value.cc
)

set(DWARF_HEADERS
    dwarf++.hh
    data.hh
    internal.hh
    small_vector.hh
)

add_library(dwarf++ STATIC ${DWARF_SOURCES} ${DWARF_HEADERS})

# Disable warnings for this external library
target_compile_options(dwarf++ PRIVATE -w)

# Link with elf++
target_link_libraries(dwarf++ PUBLIC elf++)

# Export include directory
target_include_directories(dwarf++ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set C++11 standard
set_target_properties(dwarf++ PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
