{ libsForQt5, cmake, qt5, lib }:
with libsForQt5;
mkDerivation rec {
    name = "QtRVSim";
    src = builtins.fetchGit ../../..;
    nativeBuildInputs = [ cmake ];
    buildInputs = [ qt5.qtbase qt5.qtsvg ];
    meta = {
        description = "RISC-V CPU simulator for education purposes.";
        longDescription = ''
          RISC-V CPU simulator for education purposes with pipeline and cache visualization.
          Developed at FEE CTU for computer architecturs classes.
        '';
        homepage = "https://github.com/cvut/qtrvsim";
        license = lib.licenses.gpl3Plus;
        maintainers = [ "Jakub Dupak <dev@jakubdupak.com>" ];
    };
}