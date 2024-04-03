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
