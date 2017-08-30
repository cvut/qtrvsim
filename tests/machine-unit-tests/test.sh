#!/bin/sh
TEST_NAME="machine-unit-test"

# Load common test shell functions
. "$(dirname "$0")/../test.sh"

# Build tests binary
qtmips_make sub-qtmips_machine-tests

# Run unit tests
qtmips_run qtmips_machine/tests/tst_machine || echo_fail "Test $TEST_NAME failed!"
