#!/usr/bin/env bash

set -eu

EMSCRIPTEN_VERSION=$1
QT_WASM=$2
EMSDK_PATH=$3

# If you know how to do this better, please, let me know.
[ "$(stat -c '%a' "$EMSDK_PATH")" -ne 777 ] && sudo chmod 777 -R $EMSDK_PATH
emsdk install $EMSCRIPTEN_VERSION
emsdk activate $EMSCRIPTEN_VERSION
source $EMSDK_PATH/emsdk_env.sh
mkdir -p build/wasm
pushd build/wasm
emcmake cmake ../.. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$QT_WASM" \
  -DCMAKE_FIND_ROOT_PATH="$QT_WASM" \
  -Wno-dev -G Ninja
ninja
popd