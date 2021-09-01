let
  pkgs = import <nixpkgs> { };
in
{
    qtrvsim = pkgs.libsForQt5.callPackage (import extras/packaging/nix/qtrvsim.nix) {};
}
