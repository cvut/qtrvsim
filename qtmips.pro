TEMPLATE = subdirs

SUBDIRS += \
    machine \
    os_emulation \
    assembler

!wasm: SUBDIRS += \
    machine-tests \
    cli \

SUBDIRS += \
    gui

machine.subdir = src/machine
assembler.subdir = src/assembler
os_emulation.subdir = src/os_emulation
cli.subdir = src/cli
gui.subdir = src/gui
machine-tests.subdir = src/machine/tests

cli.depends = machine assembler
gui.depends = machine os_emulation assembler
machine-tests.depends = machine
