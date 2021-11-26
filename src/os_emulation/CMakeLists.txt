project(os_emulation
        DESCRIPTION "Simple emulation of a Linux like kernel")

set(CMAKE_AUTOMOC ON)

set(os_emulation_SOURCES
        ossyscall.cpp
        )
set(os_emulation_HEADERS
        ossyscall.h
        syscall_nr.h
        target_errno.h
        )

add_library(os_emulation STATIC
        ${os_emulation_SOURCES}
        ${os_emulation_HEADERS})
target_link_libraries(os_emulation
		PRIVATE ${QtLib}::Core)