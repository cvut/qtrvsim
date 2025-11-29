# Compatibility wrapper for CMake > 3.12 where FindPythonInterp is deprecated/removed
# This allows external projects using the old module to work with newer CMake and Python3

find_package(Python3 COMPONENTS Interpreter QUIET)

if(Python3_Interpreter_FOUND)
    set(PYTHONINTERP_FOUND TRUE)
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    set(PYTHON_VERSION_STRING ${Python3_VERSION})
    set(PYTHON_VERSION_MAJOR ${Python3_VERSION_MAJOR})
    set(PYTHON_VERSION_MINOR ${Python3_VERSION_MINOR})
    set(PYTHON_VERSION_PATCH ${Python3_VERSION_PATCH})
    
    if(NOT PythonInterp_FIND_QUIETLY)
        message(STATUS "Found PythonInterp (via Python3): ${PYTHON_EXECUTABLE} (found version \"${PYTHON_VERSION_STRING}\")")
    endif()
else()
    if(PythonInterp_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find PythonInterp (missing: PYTHON_EXECUTABLE)")
    endif()
endif()
