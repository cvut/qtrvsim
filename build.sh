#!/bin/sh
set -e

ROOT="$(dirname "$(readlink -f "$0")")"

mkdir -p build
cd build

# Compile
qtchooser -run-tool=qmake -qt=5 "$ROOT" "QMAKE_RPATHDIR += ../qtmips_machine"
make sub-qtmips_cli sub-qtmips_gui # Note: we are building these to to not build tests

# Link executables to more suitable place
ln -fs qtmips_cli/qtmips_cli cli
ln -fs qtmips_gui/qtmips_gui gui
