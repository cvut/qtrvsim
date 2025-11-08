all:
	$(MAKE) -C elf
	$(MAKE) -C dwarf

install:
	$(MAKE) -C elf install
	$(MAKE) -C dwarf install

clean:
	$(MAKE) -C elf clean
	$(MAKE) -C dwarf clean

check:
	cd test && ./test.sh
