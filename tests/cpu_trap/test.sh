#!/bin/sh
TEST_NAME="cpu_trap"

# Load common test shell functions
. "$(dirname "$0")/../test.sh"

# Build cli binary
qtmips_make sub-qtmips_cli

# Compile mips binary
mips_make_test

# Run test
qtmips_run qtmips_cli/qtmips_cli --fail-match O "$TEST_DIR/cpu_trap"
