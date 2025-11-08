cmake_minimum_required(VERSION 3.10)
project(libelfin CXX)

# Set C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ELF++ library
add_subdirectory(elf)

# DWARF++ library (optional, but we include it for completeness)
add_subdirectory(dwarf)
