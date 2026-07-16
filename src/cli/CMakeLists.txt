project(cli
        LANGUAGES C CXX
        VERSION ${MAIN_PROJECT_VERSION}
        DESCRIPTION "Simulator command-line UI.")

set(CMAKE_AUTOMOC ON)

set(cli_SOURCES
        chariohandler.cpp
        main.cpp
        msgreport.cpp
        reporter.cpp
        tracer.cpp
        utilandtext.cpp
)
set(cli_HEADERS
        chariohandler.h
        msgreport.h
        reporter.h
        tracer.h
        utilandtext.h
)

add_executable(cli
        ${cli_SOURCES}
        ${cli_HEADERS})
target_link_libraries(cli
        PRIVATE ${QtLib}::Core machine os_emulation assembler)
target_compile_definitions(cli
        PRIVATE
        APP_ORGANIZATION=\"${MAIN_PROJECT_ORGANIZATION}\"
        APP_ORGANIZATION_DOMAIN=\"${MAIN_PROJECT_HOMEPAGE_URL}\"
        APP_NAME=\"${MAIN_PROJECT_NAME}\"
        APP_VERSION=\"${PROJECT_VERSION}\"
        ENV_CONFIG_FILE_NAME=\"${MAIN_PROJECT_NAME_UPPER}_CONFIG_FILE\")
set_target_properties(cli PROPERTIES
        OUTPUT_NAME "${MAIN_PROJECT_NAME_LOWER}_${PROJECT_NAME}")

# =============================================================================
# Installation
# =============================================================================

# Prior to CMake version 3.13, installation must be performed in the subdirectory,
# there the target was created. Therefore executable installation is to be found
# in corresponding CMakeLists.txt.

install(TARGETS cli
        RUNTIME DESTINATION bin)

include(../../cmake/TestingTools.cmake)

enable_testing()

add_cli_test(
        NAME stalls
        ARGS
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/stalls/program.S"
        --dump-registers
        EXPECTED_OUTPUT "tests/cli/stalls/stdout.txt"
)

add_cli_test(
        NAME machine-config
        ARGS
        --machine-config "${CMAKE_SOURCE_DIR}/tests/cli/machine-config/machine.json"
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/stalls/program.S"
        --dump-registers
        EXPECTED_OUTPUT "tests/cli/stalls/stdout.txt"
)

add_cli_test(
        NAME cycle-limit
        ARGS
        --cycle-limit 5
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/stalls/program.S"
        --dump-cycles
        EXPECTED_OUTPUT "tests/cli/limits/cycle-stdout.txt"
)

add_cli_test(
        NAME instruction-limit
        ARGS
        --pipelined
        --instruction-limit 3
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/stalls/program.S"
        --dump-cycles
        EXPECTED_OUTPUT "tests/cli/limits/instruction-stdout.txt"
)

add_custom_target(
        cli_test_limit-json
        COMMAND ${CMAKE_COMMAND} -E make_directory "Testing"
        COMMAND cli
        --cycle-limit 5
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/stalls/program.S"
        --dump-cycles
        --dump-to-json "Testing/limit-report.json"
        --disable-console-dump
        COMMAND ${CMAKE_COMMAND} -E compare_files
        "Testing/limit-report.json"
        "${CMAKE_SOURCE_DIR}/tests/cli/limits/report.json"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        DEPENDS cli
)
add_test(
        NAME cli_limit-json
        COMMAND ${CMAKE_COMMAND} --build . --target cli_test_limit-json
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_target(
        cli_test_os-exit
        COMMAND ${CMAKE_COMMAND} -E make_directory "Testing"
        COMMAND cli
        --os-emulation
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/os-exit/program.S"
        --dump-cycles
        --dump-to-json "Testing/os-exit-report.json"
        --disable-console-dump
        COMMAND ${CMAKE_COMMAND} -E compare_files
        "Testing/os-exit-report.json"
        "${CMAKE_SOURCE_DIR}/tests/cli/os-exit/report.json"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        DEPENDS cli
)
add_test(
        NAME cli_os-exit
        COMMAND ${CMAKE_COMMAND} --build . --target cli_test_os-exit
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_cli_test(
        NAME asm_error
        ARGS
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/asm-error/program.S"
)
set_tests_properties(cli_asm_error PROPERTIES WILL_FAIL TRUE)

add_cli_test(
        NAME modifiers
        ARGS
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/modifiers/program.S"
        EXPECTED_OUTPUT "tests/cli/modifiers/stdout.txt"
)

add_cli_test(
        NAME modifiers-pcrel
        ARGS
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/modifiers-pcrel/program.S"
        EXPECTED_OUTPUT "tests/cli/modifiers-pcrel/stdout.txt"
)

add_cli_test(
        NAME virtual_memory_template
        ARGS
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/virtual_memory/template/program.S"
        --dump-registers
        EXPECTED_OUTPUT "tests/cli/virtual_memory/template/stdout.txt"
)

add_cli_test(
        NAME virtual_memory_dtlb
        ARGS
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/virtual_memory/dtlb/program.S"
        --dump-registers
        EXPECTED_OUTPUT "tests/cli/virtual_memory/dtlb/stdout.txt"
)

add_cli_test(
        NAME virtual_memory_itlb
        ARGS
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/virtual_memory/itlb/program.S"
        --dump-registers
        EXPECTED_OUTPUT "tests/cli/virtual_memory/itlb/stdout.txt"
)

add_cli_test(
        NAME virtual_memory_memrw
        ARGS
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/virtual_memory/memrw/program.S"
        --dump-registers
        EXPECTED_OUTPUT "tests/cli/virtual_memory/memrw/stdout.txt"
)

add_cli_test(
        NAME virtual_memory_exec
        ARGS
        --asm "${CMAKE_SOURCE_DIR}/tests/cli/virtual_memory/exec/program.S"
        --dump-registers
        EXPECTED_OUTPUT "tests/cli/virtual_memory/exec/stdout.txt"
)