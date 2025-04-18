# Provides shell with a crosscompiler for code to simulate
with import <nixpkgs> {};
pkgsCross.riscv64-embedded.mkShell {}
