# Enable shell exit on failure
set -e

## Colored echo function ########################################################

echo_head() {
	echo -e "\e[1;34m-- $@ --\e[0m" >&2
}

echo_msg() {
	echo -e "\e[1;33m$@\e[0m" >&2
}

echo_fail() {
	echo -e "\e[1;31m$@\e[0m" >&2
	exit 1
}

## Check and init test ##########################################################

# Check if test name is defined
[ -n "$TEST_NAME" ] || echo_fail "Test name is not defined! Define TEST_NAME variable in test script."
# Report user what test is going to be executed
echo_head "Running test $TEST_NAME"

## Define some variables ########################################################

# Store base directory from which we started test script
PWD_DIR="$(pwd)"
# Define path to test sources
TEST_SRC="$(dirname "$(readlink -f "$0")")"

# Get project root.
# As we want to have script calling this anywhere, we have to do some more work to
# get project root. But we know content of root and so we can use that to locate
# it and we know that test source directory is part of tree so it's under root.
PROJECT_ROOT="$TEST_SRC"
while [ -n "$PROJECT_ROOT" ] && ! [ \
		-f "$PROJECT_ROOT/LICENSE" -a \
		-f "$PROJECT_ROOT/README.md" -a \
		-d "$PROJECT_ROOT/qtmips_gui" -a \
		-d "$PROJECT_ROOT/qtmips_cli" -a \
		-d "$PROJECT_ROOT/qtmips_machine" \
		]; do
	PROJECT_ROOT="${PROJECT_ROOT%/*}"
done
[ -n "$PROJECT_ROOT" ] || echo_fail "Project root not found!!"

# Define root for all tests
# We are little bit clever about it. If we detect that we are in project tree we
# will use same directory every time. Otherwise we use current working directory
# where we create test_dir directory.
if [ -n "${PWD_DIR#$PROJECT_ROOT}" ]; then
	TEST_DIR_ROOT="$PROJECT_ROOT/test_dir"
else
	TEST_DIR_ROOT="$PWD_DIR/test_dir"
fi

# Directory where qtmips will be build
BUILD_DIR="$TEST_DIR_ROOT/build"

# Directory where test should be build and executed
TEST_DIR="$TEST_DIR_ROOT/$TEST_NAME"

## Helper functions for building and running project ############################

qtmips_make() {
	mkdir -p "$BUILD_DIR"
	local ORIG="$(pwd)"
	cd "$BUILD_DIR"
	/usr/lib64/qt5/bin/qmake "$PROJECT_ROOT" || echo_fail "QtMips qmake failed!"
	make "$@" || echo_fail "QtMips build failed! (target: $@)"
	cd "$ORIG"
}

qtmips_run() {
	local BIN="$BUILD_DIR/$1"
	shift
	LD_LIBRARY_PATH="$BUILD_DIR/qtmips_machine" "$BIN" "$@"
}

## Mips elf binary compilation ##################################################

MIPS_COMPILER="$TEST_DIR_ROOT/mips-qtmips-elf"

mips_compiler() {
	# Skip if we have compiler already
	[ ! -d "$MIPS_COMPILER" ] || return 0
	# Otherwise compile it
	mkdir -p "$TEST_DIR_ROOT"
	local ORIG="$(pwd)"
	cd "$TEST_DIR_ROOT"
	"$PROJECT_ROOT"/compiler/compile.sh
	cd "$ORIG"
}

mips_make_test() {
	mips_compiler
	mkdir -p "$TEST_DIR"
	PATH="$PATH:$MIPS_COMPILER/bin" make -C "$TEST_SRC" O="$TEST_DIR" MIPS_PREFIX="mips-qtmips-elf-" "$@"
}
