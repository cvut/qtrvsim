project(polyfills
        DESCRIPTION "Helper code to unify access to compiler intrinsics and
                     provide fallback implementations.")

set(polyfills_HEADERS
        endian_detection.h
        byteswap.h
        )

# =============================================================================
# Target for usage of this header library
# - this is header only library and it is implicitly available in the whole
#   project
# =============================================================================

# =============================================================================
# Tests
# - It is highly recommended to run these tests anytime you use a different
#   compiler or platform as this code is platform/compiler specific.
# =============================================================================

enable_testing()
set(CMAKE_AUTOMOC ON)

# Put tests here...