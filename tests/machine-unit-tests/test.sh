#!/usr/bin/env bash
TEST_NAME="machine-unit-test"

# Load common test shell functions
. "$(dirname "$0")/../test_lib.sh"

# Build tests binary
qtmips_make sub-src-machine-tests
# Build test data
mips_make_test

cd "$TEST_DIR"

# Run unit tests
qtmips_run src/machine/tests/tst_machine
