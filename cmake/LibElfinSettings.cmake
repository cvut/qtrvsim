if(POLICY CMP0148)
    cmake_policy(SET CMP0148 OLD)
endif()

# Libelfin requires Threads for dwarf++ library
find_package(Threads REQUIRED)

