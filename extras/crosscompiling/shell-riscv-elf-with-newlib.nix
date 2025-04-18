# Provides shell with a crosscompiler for code to simulate
with import <nixpkgs> {
  crossSystem = {
    config = "riscv32-none-elf";
    libc = "newlib";
    gcc.arch = "rv32g";
  };
};

mkShell {
  buildInputs = [ ]; # your dependencies here
}
