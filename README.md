QtMips
======
MIPS CPU simulator for education purposes.

Dependencies
------------
* Qt 5
* elfutils (libelf might works too but you can have problems with it)

General compilation
-------------------
To compile whole project just run these commands:
```
qmake /path/to/qtmips
make
```
Where `/path/to/qtmips` is path to this project root.

(Be sure to use qt5 qmake.)

Compilation for local execution
-------------------------------
Because simulator it self is implemented as library you need to have that library
in path where loader can found it. Which is not commonly a case, so you have to
compile it with following additional option:
```
qmake /path/to/qtmips "QMAKE_RPATHDIR += ../qtmips_machine"
make
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
* Branch-likeli instructions (they are marked as obsolete)
* Complete binary instruction check (we check only minimal set of bites to decode
  instruction, we don't check if zero sections are really zero unless we need it).
