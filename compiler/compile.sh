#!/usr/bin/env bash
# This script compiles compilation tools for mips to be used with qtmips
set -e

INST_PREFIX="$(pwd)"

INST_CT_PREFIX="$INST_PREFIX/ct-ng"
mkdir -p "$INST_CT_PREFIX"

# First update git submodule
pushd "$(dirname "$0")" >/dev/null
git submodule update crosstool-ng

pushd crosstool-ng >/dev/null

# Now compile it
# TODO don't compile it in place?
./bootstrap
./configure --prefix="$INST_CT_PREFIX"
make
make install
# TODO do cleanups?

popd >/dev/null

popd >/dev/null

# Copy configuration
cp "$(dirname "$0")/config" ct-ng/.config

pushd ct-ng >/dev/null

# And compile
./bin/ct-ng oldconfig
CT_PREFIX="$INST_PREFIX" ./bin/ct-ng build

popd >/dev/null

# Cleanup installed crosstool-ng
rm -rf ct-ng
