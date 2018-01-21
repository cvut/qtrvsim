#!/bin/sh
TEST_NAME="registers"

# Load common test shell functions
. "$(dirname "$0")/../test.sh"

# Build cli binary
qtmips_make sub-qtmips_cli

# Compile mips binary
mips_make_test

# Run test
qtmips_run qtmips_cli/qtmips_cli --trace-gp '*' "$TEST_DIR/registers" \
	|| echo_fail "Test $TEST_NAME failed!"
