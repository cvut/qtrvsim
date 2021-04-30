project(common
        DESCRIPTION "Utils shared between multiple subprojects.")

add_subdirectory(polyfills)

set(common_HEADERS
        endian.h
        logging.h
        logging_format_colors.h
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
