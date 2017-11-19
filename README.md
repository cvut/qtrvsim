QtMips
======
MIPS CPU simulator for education purposes.

Dependencies
------------
* Qt 5 (qtmips\_gui can be compiled with Qt4 but not qtmips\_cli)
* elfutils (libelf works too but you might have problems with it)

Compilation
-----------
To compile whole project just run these commands:
```
qmake /path/to/qtmips
make
```
Where `/path/to/qtmips` is path to this project root.

Running
-------
Because simulator it self is implemented as library you need to have that library
in common path where loader can found it. Which is not commonly a case, so you can
also run it using this command (in case of cli version, gui version is the same)
```
LD_LIBRARY_PATH=qtmips_machine qtmips_cli/qtmips_cli
```

Tests
-----
There are two types of tests in QtMips. One type are unit tests for simulator it
self and second one are integration tests with command line client and real
compiled elf binaries. All these tests can be executed using script
`tests/run-all.sh` or one by one by running respective `test.sh` scripts.

Source files for unit tests can be found in path `qtmips_machine/tests` and
integration tests are located in `tests` directory.

Not implemented features
------------------------
These are features that are not implemented and are not currently being planned as
being so.

* Privileged instructions and all features dependent on it
* Coprocessors (so no floating point unit nor any other type)
* Peripherals
* Memory access stall (stalling execution because of cache miss would be pretty
  annoying for users so difference between cache and memory is just in collected
  statistics)
* Interrupts and exceptions (if exception occurs then machine execution is halted)
