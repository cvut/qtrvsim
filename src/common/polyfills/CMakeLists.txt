project(polyfills
        DESCRIPTION "Helper code to unify access to compiler intrinsics and
                     provide fallback implementations.")

# =============================================================================
# Target for usage of this header library
# =============================================================================

add_library(polyfills INTERFACE)
target_include_directories(polyfills INTERFACE .)

# =============================================================================
# Tests
# - It is highly recommended to run these tests anytime you use a different
#   compiler or platform as this code is platform/compiler specific.
# =============================================================================

enable_testing()
set(CMAKE_AUTOMOC ON)

# Put tests here...