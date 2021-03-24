project(qtmips_osemu
        DESCRIPTION "Simple emulation of a Linux like kernel")

set(CMAKE_AUTOMOC ON)

set(qtmips_osemu_SOURCES
        ossyscall.cpp
        )
set(qtmips_osemu_HEADERS
        ossyscall.h
        syscall_nr.h
        target_errno.h
        )

add_library(qtmips_osemu OBJECT
        ${qtmips_osemu_SOURCES}
        ${qtmips_osemu_HEADERS})
target_link_libraries(qtmips_osemu
        PRIVATE Qt5::Core)