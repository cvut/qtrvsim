#!/bin/sh

mkdir -p build

ROOT="$(dirname "$(readlink -f "$0")")"

cd build
qtchooser -run-tool=qmake -qt=5 "$ROOT"

make sub-qtmips_cli sub-qtmips_gui
