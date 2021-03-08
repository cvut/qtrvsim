project(qtmips_asm)

set(CMAKE_AUTOMOC ON)

set(qtmips_asm_SOURCES
        fixmatheval.cpp
        simpleasm.cpp
        )
set(qtmips_asm_HEADERS
        fixmatheval.h
        messagetype.h
        simpleasm.h
        )

add_library(qtmips_asm ${qtmips_asm_SOURCES} ${qtmips_asm_HEADERS})
target_link_libraries(qtmips_asm Qt5::Core)
