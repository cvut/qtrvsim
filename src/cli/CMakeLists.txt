project(qtmips_cli)

set(CMAKE_AUTOMOC ON)

set(qtmips_cli_SOURCES
        chariohandler.cpp
        main.cpp
        msgreport.cpp
        reporter.cpp
        tracer.cpp
        )
set(qtmips_cli_HEADERS
        chariohandler.h
        msgreport.h
        reporter.h
        tracer.h
        )

add_executable(qtmips_cli ${qtmips_cli_SOURCES} ${qtmips_cli_HEADERS})
target_link_libraries(qtmips_cli Qt5::Core qtmips_machine qtmips_asm)
