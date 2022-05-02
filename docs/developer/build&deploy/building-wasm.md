# Building and deploying WebAssembly release of the simulator

## Jump start

WASM release is automatically build by the GitHub actions. Resulting files are available in `Actions`
/`<detail of build you want to use>`, section `Artifacts`, file `target-wasm-Linux-qtx.xx.x`. Download the files and
skip to deploy. Details on the build process can be found in `.github/workflows/cmake.yml`.

## Jump start 2

Install `emsdk` and `aqt` (details below).

Use prepared helper make in the root of the project.

```shell
make wasm # build
make wasm-clean # clean wasm build
make wasm-clean-deap # also clean cached external libraries - when changing compile settings, of when it just does not work ;)
make wasm-install-qt # install appropriate qt for wasm using aqt
```

Behavior of this commands is customized to local system in `.dev-config.local.mk`. The file is provided as a template
and further changes are ignored.

## Dependencies

- WASM compiler (Emscripten/EMSDK)
- WASM compiled Qt
- bash compatible shell (not fish, unfortunately)

Steps:

- Install `emsdk` toolkit. Installing just the Emscripten compiler might work, but I would discourage it.
- Choose version of the toolkit and run `emsdk activate <version>`. You can choose freely as long as all your components
  are compiled with the compatible version. Qt5 distributed release is build with version `1.39.8`. Some distributions
  provide packages like `qt5-wasm`. It is imperative to compile the project with the same toolchain, otherwise it might
  not link.
- Source the toolkit as suggested by the output of the activate command.
- Download or compile qt to wasm. Common way is to store it in `/opt/`. I use an unofficial qt downloader
  [`aqtinstall`](https://pypi.org/project/aqtinstall/).
  - `python -m aqt install 5.12.2 linux desktop wasm_32 --outputdir "/opt/qt"`

## Build

- Move to build directory a build directory (I use`build/wasm`) and
  run (replace the paths accordingly and use your favorite buildsystem):
  ```shell
  emcmake cmake ../..
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_PREFIX_PATH=/opt/qt/5.13.1/wasm_32/ 
        -DCMAKE_FIND_ROOT_PATH=/opt/qt/5.13.1/wasm_32/ -Wno-dev -G Ninja
  ```
- Move the resulting `.js` and `.wasm` files to `target/wasm` along with all content of `data/wasm`. It contains
  loader script, icons and other support files.

## Deploy

- Move the contents of `target/wasm` to a webserver. Done.
- Note: the webserver must use https due to CORS.

## Tricks

### Fish and co.

If you wish to use bash incompatible shell. Run bash, source the file, and the run the shell with the environment of the
bash. 
