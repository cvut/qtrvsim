# CMake generated Testfile for 
# Source directory: /home/tux/qtrvsim/src/machine
# Build directory: /home/tux/qtrvsim/cmake-build-debug-wsl/src/machine
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(alu "/home/tux/qtrvsim/cmake-build-debug-wsl/target/alu_test")
set_tests_properties(alu PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;99;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(registers "/home/tux/qtrvsim/cmake-build-debug-wsl/target/registers_test")
set_tests_properties(registers PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;112;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(memory "/home/tux/qtrvsim/cmake-build-debug-wsl/target/memory_test")
set_tests_properties(memory PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;130;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(cache "/home/tux/qtrvsim/cmake-build-debug-wsl/target/cache_test")
set_tests_properties(cache PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;156;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(instruction "/home/tux/qtrvsim/cmake-build-debug-wsl/target/instruction_test")
set_tests_properties(instruction PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;170;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(program_loader "/home/tux/qtrvsim/cmake-build-debug-wsl/target/program_loader_test")
set_tests_properties(program_loader PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;191;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
add_test(core "/home/tux/qtrvsim/cmake-build-debug-wsl/target/core_test")
set_tests_properties(core PROPERTIES  _BACKTRACE_TRIPLES "/home/tux/qtrvsim/src/machine/CMakeLists.txt;227;add_test;/home/tux/qtrvsim/src/machine/CMakeLists.txt;0;")
