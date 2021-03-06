#!/usr/bin/env bash
TEST_NAME="cpu_trap"

# Load common test shell functions
. "$(dirname "$0")/../test_lib.sh"

# Build cli binary
qtmips_make sub-src-cli

# Compile mips binary
mips_make_test

# Run test
qtmips_run src/cli/cli --fail-match O "$TEST_DIR/cpu_trap"
qtmips_run src/cli/cli --pipelined --fail-match O "$TEST_DIR/cpu_trap"
