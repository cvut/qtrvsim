{ lib, stdenv, cmake, wrapQtAppsHook, qtbase, qtsvg }:
stdenv.mkDerivation {
    name = "QtRVSim";
    src = builtins.fetchGit ../../..;
    nativeBuildInputs = [ cmake wrapQtAppsHook ];
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