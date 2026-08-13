{ lib, stdenv, cmake, wrapQtAppsHook, python3, qtbase, qtsvg, qtrvsimSource ? ../../.. }:
let
    sourceRoot = toString qtrvsimSource;
    source = lib.cleanSourceWith {
        src = qtrvsimSource;
        filter = path: type:
            let
                relativePath = lib.removePrefix "${sourceRoot}/" (toString path);
                topLevel = builtins.head (lib.splitString "/" relativePath);
            in
                lib.cleanSourceFilter path type
                && !(builtins.elem topLevel [ "build" "CMakeFiles" "test_dir" ])
                && !(builtins.elem relativePath [
                    ".dev-config.mk"
                    "compile_commands.json"
                    "src/project_info.h"
                ]);
    };
in
stdenv.mkDerivation {
    name = "QtRVSim";
    src = source;
    nativeBuildInputs = [ cmake wrapQtAppsHook python3 ];
    buildInputs = [ qtbase qtsvg ];
    meta = {
        description = "RISC-V CPU simulator for education purposes.";
        longDescription = ''
          RISC-V CPU simulator for education purposes with pipeline and cache visualization.
          Developed at FEE CTU for computer architecture classes.
        '';
        homepage = "https://github.com/cvut/qtrvsim";
        license = lib.licenses.gpl3Plus;
        maintainers = [ "Jakub Dupak <dev@jakubdupak.com>" ];
    };
}