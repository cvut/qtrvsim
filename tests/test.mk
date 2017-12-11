MAKEFLAGS += --no-builtin-rules

# Output path
O ?= .

#MIPS_CFLAGS += -Wall
MIPS_LDFLAGS += -nostdlib -nodefaultlibs -nostartfiles -Wl,-Ttext,0x80020000

.PHONY: all
all::
	@

.PHONY: clean
clean::
	@


define MIPS_ELF

ifndef MIPS_PREFIX
	$$(error Toolchain prefix have to be passed in MIPS_PREFIX variable)
endif

OBJ_$(1):=$$(patsubst %.S,$$(O)/%.o,$$(SRC_$(1)))
$$(info $$(OBJ_$(1)))

all:: $$(O)/$(1)

$$(O)/$(1): $$(OBJ_$(1))
	$$(MIPS_PREFIX)gcc $$(MIPS_LDFLAGS) -o $$@ $$^

$$(OBJ_$(1)): $$(O)/%.o: %.S
	$$(MIPS_PREFIX)gcc $$(MIPS_CFLAGS) -c -x assembler-with-cpp -o $$@ $$<

clean::
	$(RM) $$(OBJ_$(1))
	$(RM) $$(O)/$(1)

endef
