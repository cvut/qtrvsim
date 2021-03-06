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

## Parse script arguments #######################################################
TOOLCHAIN_SYS=false
TOOLCHAIN="mips-elf-"

# Enviromental variable passing additional options to qmake, like compiler to use.
QMAKE_OPTIONS=${QMAKE_OPTIONS:-""}

while [ $# -gt 0 ]; do
	case "$1" in
		-\?|-h|--help)
			echo "QtMips test script ($TEST_NAME)"
			echo "Usage: $0 [OPTION]..."
			echo
			echo "Options:"
			echo "  -s, --system-toolchain"
			echo "    Don't compile it's own toolchain. Use system toolchain instead"
			echo "  --toolchain PREFIX"
			echo "    What toolchain prefix should be used when system toolchain is used."
			echo "    In default mips-elf- is used."
			echo "  -h, -?, --help"
			echo "    Print this help text."
			exit 0
			;;
		-s|--system-toolchain)
			TOOLCHAIN_SYS=true
			;;
		--toolchain)
			TOOLCHAIN="$2"
			shift
			;;
	esac
	shift
done

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
		-d "$PROJECT_ROOT/src/gui" -a \
		-d "$PROJECT_ROOT/src/cli" -a \
		-d "$PROJECT_ROOT/src/machine" \
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

# TODO verify that we really have system toolchain if configured.

## Helper functions for building and running project ############################

qtmips_make() {
	mkdir -p "$BUILD_DIR"
	local ORIG="$(pwd)"
	cd "$BUILD_DIR"
	qmake "$PROJECT_ROOT" "CONFIG+=debug" $QMAKE_OPTIONS || echo_fail "QtMips qmake failed!"
	make "$@" "-j$(nproc)" -s  || echo_fail "QtMips build failed! (target: $@)"
	cd "$ORIG"
}

qtmips_run() {
	local BIN="$BUILD_DIR/$1"
	shift
	LD_LIBRARY_PATH="$BUILD_DIR/src/machine" "$BIN" "$@" || \
		echo_fail "QtMips execution exited with non-zero code ($?): $@"
}

## Mips elf binary compilation ##################################################

MIPS_COMPILER="$TEST_DIR_ROOT/mips-qtmips-elf"

_mips_compiler() {
	export PATH="$PATH:$MIPS_COMPILER/bin"
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
	local PREFIX="mips-qtmips-elf-"
	if $TOOLCHAIN_SYS; then
		PREFIX="$TOOLCHAIN"
	else
		_mips_compiler
	fi
	mkdir -p "$TEST_DIR"
	make -C "$TEST_SRC" O="$TEST_DIR" MIPS_PREFIX="$PREFIX" "$@"
}
