#!/usr/bin/env bash

# exit immediately if a command exits with a non-zero status
set -e

# determine project root
ROOT=`pwd`/$(dirname -- "$(which "$0")")

# clean and create build folder
echo "creating build directory ..."
rm -rf build
mkdir -p build
cd build
echo ""

# compile and apply macOS specifics:
#   1. add libelf from Homebrew
#     note 1: both /usr/local/include and /usr/local/include/libelf must be added to INCLUDEPATH
#             because libelf.h is imported without folder prefix
#     note 2: we could use /usr/local/include, /usr/local/include/libelf and /usr/local/lib
#           but just to be sure we use exact installation path (may break if new version is released and path changes)
#   2. set QMAKE_MACOSX_DEPLOYMENT_TARGET to get rid of the following warning (may not be necessary):
#     ld: warning: object file (/usr/local/Cellar/libelf/...) was built for newer OSX version (10.14) than being linked (10.12)
#   3. use recommendations from https://doc.qt.io/qt-5.9/osx-deployment.html
#  qmake docs: https://doc.qt.io/qt-5/qmake-variable-reference.html
echo "running qmake ..."
qmake \
	-config release \
	-recursive \
	"$ROOT" \
	"QMAKE_RPATHDIR += ../qtmips_machine ../qtmips_osemu ../qtmips_asm" \
	"INCLUDEPATH += /usr/local/Cellar/libelf/0.8.13_1/include/libelf /usr/local/Cellar/libelf/0.8.13_1/include" \
	"LIBS += -L/usr/local/Cellar/libelf/0.8.13_1/lib -lelf" \
	"QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14"
echo ""

echo "running sub-qtmips_cli sub-qtmips_gui ..."
make sub-qtmips_cli sub-qtmips_gui # Note: we are building these to to not build tests
echo ""

# add Qt framework (adds Qt static libs as framework and do some other tweaks)
echo "running macdeployqt ..."
macdeployqt qtmips_gui/qtmips_gui.app -verbose=2
echo ""

echo "creating qtmips.zip ..."
cd qtmips_gui
zip -yr ../qtmips-macos.zip qtmips_gui.app
cd ..
zip -j qtmips-macos.zip qtmips_cli/qtmips_cli
echo ""
