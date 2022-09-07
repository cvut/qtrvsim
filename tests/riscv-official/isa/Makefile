#=======================================================================
# Makefile for riscv-tests/isa
#-----------------------------------------------------------------------

XLEN ?= 64

src_dir := .

isa_dir := ../riscv-tests/isa

ifeq ($(XLEN),64)
include $(isa_dir)/rv64ui/Makefrag
include $(isa_dir)/rv64ua/Makefrag
include $(isa_dir)/rv64si/Makefrag
include $(isa_dir)/rv64mi/Makefrag
include $(isa_dir)/rv64um/Makefrag
endif
include $(isa_dir)/rv32ui/Makefrag
include $(isa_dir)/rv32ua/Makefrag
include $(isa_dir)/rv32si/Makefrag
include $(isa_dir)/rv32mi/Makefrag
include $(isa_dir)/rv32um/Makefrag

default: clean all

#--------------------------------------------------------------------
# Build rules
#--------------------------------------------------------------------

include toolchain_setup

vpath %.S $(isa_dir) $(self_test)

#------------------------------------------------------------
# Build assembly tests

%.dump: %
	$(RISCV_OBJDUMP) elf/$< > dump/$@

%.out: %
	$(RISCV_SIM) --isa=rv64gc $< 2> $@

%.out32: %
	$(RISCV_SIM) --isa=rv32gc $< 2> $@

define compile_template



$$($(1)_p_tests): $(1)-p-%: $(1)/%.S
	$$(RISCV_COMPILER) $(2) $$(RISCV_COMPILER_OPTS) -I$(src_dir)/../env/p -I$(isa_dir)/macros/scalar -T$(src_dir)/../env/p/link.ld $$< -o elf/$$@
$(1)_tests += $$($(1)_p_tests)

$(1)_tests_dump = $$(addsuffix .dump, $$($(1)_tests))

$(1): $$($(1)_tests_dump)

.PHONY: $(1)

COMPILER_SUPPORTS_$(1) := $$(shell $$(RISCV_COMPILER) $(2) -c -x c /dev/null -o /dev/null 2> /dev/null; echo $$$$?)

ifeq ($$(COMPILER_SUPPORTS_$(1)),0)
tests += $$($(1)_tests)
endif

endef
$(eval $(call compile_template,rv32ui, $(MARCH_OPTS_32)))
$(eval $(call compile_template,rv32um, $(MARCH_OPTS_32)))
$(eval $(call compile_template,rv32ua, $(MARCH_OPTS_32)))
$(eval $(call compile_template,rv32si, $(MARCH_OPTS_32)))
$(eval $(call compile_template,rv32mi, $(MARCH_OPTS_32)))
ifeq ($(XLEN),64)
$(eval $(call compile_template,rv64ui, $(MARCH_OPTS_64)))
$(eval $(call compile_template,rv64um, $(MARCH_OPTS_64)))
$(eval $(call compile_template,rv64ua, $(MARCH_OPTS_64)))
$(eval $(call compile_template,rv64si, $(MARCH_OPTS_64)))
$(eval $(call compile_template,rv64mi, $(MARCH_OPTS_64)))
endif

tests_dump = $(addsuffix .dump, $(tests))
tests_hex = $(addsuffix .hex, $(tests))
tests_out = $(addsuffix .out, $(filter rv64%,$(tests)))
tests32_out = $(addsuffix .out32, $(filter rv32%,$(tests)))

run: $(tests_out) $(tests32_out)

junk += $(tests) $(tests_dump) $(tests_hex) $(tests_out) $(tests32_out)

#------------------------------------------------------------

self-tests:
	cd selftests && make

all: $(tests_dump) self-tests

#------------------------------------------------------------
# Clean up

clean:
	rm -rf $(junk)
	rm -fr elf/*
	rm -fr dump/*
	rm -fr $(self_test)/elf/*
