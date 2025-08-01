# CMake generated Testfile for 
# Source directory: /home/tux/qtrvsim/src/machine
# Build directory: /home/tux/qtrvsim/cmake-build-debug-wsl/src/machine
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(alu "/home/tux/qtrvsim/cmake-build-debug-wsl/target/alu_test")
set_tests_properties(alu PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;103;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(registers "/home/tux/qtrvsim/cmake-build-debug-wsl/target/registers_test")
set_tests_properties(registers PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;116;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(memory "/home/tux/qtrvsim/cmake-build-debug-wsl/target/memory_test")
set_tests_properties(memory PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;134;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(cache "/home/tux/qtrvsim/cmake-build-debug-wsl/target/cache_test")
set_tests_properties(cache PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;160;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(instruction "/home/tux/qtrvsim/cmake-build-debug-wsl/target/instruction_test")
set_tests_properties(instruction PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;174;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(program_loader "/home/tux/qtrvsim/cmake-build-debug-wsl/target/program_loader_test")
set_tests_properties(program_loader PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;195;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(core "/home/tux/qtrvsim/cmake-build-debug-wsl/target/core_test")
set_tests_properties(core PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;231;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
