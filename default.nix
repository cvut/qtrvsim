let
  pkgs = import <nixpkgs> { };
in
{
    qtrvsim = pkgs.callPackage (import extras/packages/nix/qtrvsim.nix) { };
}
