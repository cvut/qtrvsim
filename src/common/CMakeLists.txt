project(common
		DESCRIPTION "Utils shared between multiple subprojects.")

add_subdirectory(polyfills)

set(common_HEADERS
		endian.h
		string_utils.h
		logging.h
		logging_format_colors.h
		containers/cvector.h
		math/bit_ops.h
		memory_ownership.h
		type_utils/lens.h
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

add_custom_target(common_unit_tests
		DEPENDS mulh64_test)
