project(qtmips_machine
        DESCRIPTION "The actual simulator as a library. Link with an UI of your choice.")

set(CMAKE_AUTOMOC ON)

set(qtmips_machine_SOURCES
        alu.cpp
        cop0state.cpp
        core.cpp
        instruction.cpp
        machine.cpp
        machineconfig.cpp
        memory/backend/lcddisplay.cpp
        memory/backend/memory.cpp
        memory/backend/peripheral.cpp
        memory/backend/peripspiled.cpp
        memory/backend/serialport.cpp
        memory/cache/cache.cpp
        memory/cache/cache_policy.cpp
        memory/frontend_memory.cpp
        memory/memory_bus.cpp
        programloader.cpp
        registers.cpp
        simulator_exception.cpp
        symboltable.cpp
        )

set(qtmips_machine_HEADERS
        alu.h
        cop0state.h
        core.h
        instruction.h
        machine.h
        machineconfig.h
        machinedefs.h
        memory/address.h
        memory/backend/backend_memory.h
        memory/backend/lcddisplay.h
        memory/backend/memory.h
        memory/backend/peripheral.h
        memory/backend/peripspiled.h
        memory/backend/serialport.h
        memory/cache/cache.h
        memory/cache/cache_policy.h
        memory/cache/cache_types.h
        memory/frontend_memory.h
        memory/memory_bus.h
        memory/memory_utils.h
        programloader.h
        registers.h
        register_value.h
        simulator_exception.h
        symboltable.h
        utils.h
        machine_global.h
        )
set(qtmips_machine_TESTS
        tests/data/cache_test_performance_data.h
        tests/tst_machine.h
        tests/utils/integer_decomposition.h
        tests/testalu.cpp
        tests/testcache.cpp
        tests/testcore.cpp
        tests/testinstruction.cpp
        tests/testmemory.cpp
        tests/testprogramloader.cpp
        tests/testregisters.cpp
        tests/tst_machine.cpp
        )


# Object library is preferred, because the library archive is never really
# needed. This option skips the archive creation and links directly .o files.
add_library(qtmips_machine OBJECT
        ${qtmips_machine_SOURCES}
        ${qtmips_machine_HEADERS})
target_link_libraries(qtmips_machine
        PRIVATE Qt5::Core
        PUBLIC libelf)

if (NOT ${WASM})
    target_precompile_headers(qtmips_machine
            PUBLIC ${qtmips_machine_HEADERS})

    # Machine tests (not available on WASM)
    add_executable(qtmips_machine_unit_tests ${qtmips_machine_TESTS})
    target_link_libraries(qtmips_machine_unit_tests
            PRIVATE qtmips_machine Qt5::Core Qt5::Test)

    add_test(NAME qtmips_machine_unit_tests
            COMMAND qtmips_machine_unit_tests)
endif ()
