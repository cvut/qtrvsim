project(cli
        DESCRIPTION "Simulator command-line UI.")

set(CMAKE_AUTOMOC ON)

set(cli_SOURCES
        chariohandler.cpp
        main.cpp
        msgreport.cpp
        reporter.cpp
        tracer.cpp
        )
set(cli_HEADERS
        chariohandler.h
        msgreport.h
        reporter.h
        tracer.h
        )

add_executable(cli
        ${cli_SOURCES}
        ${cli_HEADERS})
target_link_libraries(cli
        PRIVATE Qt5::Core machine assembler)
