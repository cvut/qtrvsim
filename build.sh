#!/usr/bin/env bash
set -e

ROOT="$(dirname "$(readlink -f "$0")")"
BUILDDIR=${BUILDDIR:-build}
# Run qmake in debug mode.
# Uses enviromental variable for convenience (permanent setting).
DEBUG=${DEBUG:-false}
# Extra debug options passed to qmake. For example use different compiler.
# Uses enviromental variable for convenience (permanent setting).
QMAKE_OPTIONS=${QMAKE_OPTIONS:-""}

mkdir -p "$BUILDDIR"
cd "$BUILDDIR"


# Compile
if $DEBUG
then
  echo "Running debug build with options: $QMAKE_OPTIONS..."
  qmake .. -recursive "CONFIG += debug force_debug_info" $QMAKE_OPTIONS
else
  echo "Running release build..."
  qmake .. -recursive
fi

make "-j$(nproc)" -s

cd "$ROOT"
# Link executables to more suitable place
ln -fs "$BUILDDIR/qtmips_cli/qtmips_cli" cli
ln -fs "$BUILDDIR/qtmips_gui/qtmips_gui" gui
