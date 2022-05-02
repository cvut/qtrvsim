# This file provides local configuration to helper Makefile to speedup common development tasks
# The given values are provided as defaults and changes are ignored by git.

BUILD_DIR=./build

########################################################################################################################
# WASM
########################################################################################################################

# Wasm edition build directory
WASM_BUILD_DIR=$(BUILD_DIR)/wasm
#
EMSDK_PATH=/usr/lib/emsdk
# Version of the wasm compiler - must be carefully matched with QT version
EMSCRIPTEN_VERSION=1.39.8
# Destination for aqt installer
QT_INSTALL_DIR=/opt/qt
# Version of QT used to build wasm edition
QT_WASM_VERSION=5.15.2
# Path to QT installation intended for wasm build
QT_WASM_PATH=/opt/qt/$(QT_WASM_VERSION)/wasm_32