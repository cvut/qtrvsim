#!/bin/sh

mkdir -p build

ROOT="$(dirname "$(readlink -f "$0")")"

cd build
/usr/lib/qt5/bin/qmake "$ROOT"

make sub-qtmips_cli sub-qtmips_gui
