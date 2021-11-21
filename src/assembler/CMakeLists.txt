project(assembler
        DESCRIPTION "Simple assembler library to use in the UI.")

set(CMAKE_AUTOMOC ON)

set(assembler_SOURCES
        fixmatheval.cpp
        simpleasm.cpp
        )
set(assembler_HEADERS
        fixmatheval.h
        messagetype.h
        simpleasm.h
        )

add_library(assembler STATIC
        ${assembler_SOURCES}
        ${assembler_HEADERS})
target_link_libraries(assembler
		PRIVATE ${QtLib}::Core)
