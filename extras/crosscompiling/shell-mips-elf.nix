# Provides shell with a crosscompiler for code to simulate
with import <nixpkgs> {
  crossSystem = {
    config = "mips-elf";
  };
};

mkShell {
  buildInputs = [ ]; # your dependencies here
}