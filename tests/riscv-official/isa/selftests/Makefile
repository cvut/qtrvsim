#=======================================================================
# Makefile for riscv-tests/isa
#-----------------------------------------------------------------------

XLEN ?= 64

src_dir := ../.

isa_dir := ../../riscv-tests/isa

self_test := .
self_tests_t := $(wildcard tests/*.S)
self_tests := $(self_tests_t:tests/%=%)
elf_tests := $(self_tests:.S=.elf)

default: clean self

#--------------------------------------------------------------------
# Build rules
#--------------------------------------------------------------------

include ../toolchain_setup

vpath %.S $(self_test)

#------------------------------------------------------------
# Build assembly tests

self: $(elf_tests)

%32.elf: tests/%32.S
	$(RISCV_COMPILER) $(MARCH_OPTS_32) $(RISCV_COMPILER_OPTS) -I$(src_dir)/../env/p -I$(isa_dir)/macros/scalar -T$(src_dir)/../env/p/link.ld $< -o $(self_test)/elf/$*32

%64.elf: tests/%64.S
	$(RISCV_COMPILER) $(MARCH_OPTS_64) $(RISCV_COMPILER_OPTS) -I$(src_dir)/../env/p -I$(isa_dir)/macros/scalar -T$(src_dir)/../env/p/link.ld $< -o $(self_test)/elf/$*64
#------------------------------------------------------------
# Clean up

clean:
	rm -fr elf/*
