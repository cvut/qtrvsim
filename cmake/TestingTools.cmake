# Helper functions for integration testing

# Creates a new CLI test. The test consists of two parts: the first runs the command and checks nonzero return value,
# the second compares the stdout with provided files. Currently there diff is not displayed as cmake does not provide a
# portable way to do that.
#
# TODO: show diff whenever available.
#
# NOTE:
#   If CLI was build in debug mode (which is recommended) the test will run with sanitizers and any memory errors
#   including memory leaks will fail the test.
#
# Usage:
#   add_cli_test(
#		NAME <name>
#		ARGS
#			--asm "${CMAKE_SOURCE_DIR}/tests/cli/<name>/program.S"
#			<other CLI commands>
#		EXPECTED_OUTPUT "tests/cli/<name>/stdout.txt"
#   )

function(add_cli_test)
	cmake_parse_arguments(
			CLI_TEST
			""
			"NAME;EXPECTED_OUTPUT"
			"ARGS"
			${ARGN}
	)
	add_custom_target(
			cli_test_${CLI_TEST_NAME}
			COMMAND ${CMAKE_COMMAND} -E make_directory "Testing"
			COMMAND cli ${CLI_TEST_ARGS} > "Testing/stall_test.out"
			COMMAND ${CMAKE_COMMAND} -E compare_files "Testing/stall_test.out"
			"${CMAKE_SOURCE_DIR}/${CLI_TEST_EXPECTED_OUTPUT}"
			WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
			DEPENDS cli
	)

	add_test(
			NAME "cli_${CLI_TEST_NAME}"
			COMMAND ${CMAKE_COMMAND} --build . --target "cli_test_${CLI_TEST_NAME}"
			WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endfunction()
