project(polyfills
		DESCRIPTION "Helper code to unify access to compiler intrinsics and
                     provide fallback implementations.")

set(polyfills_HEADERS
		endian_detection.h
		mulh64.h
		clz32.h
		byteswap.h
		qstring_hash.h
		qt5/qfontmetrics.h
		qt5/qlinef.h
		qt5/qtableview.h
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

if(NOT "${WASM}")
	set(CMAKE_AUTOMOC ON)

	add_executable(mulh64_test
			mulh64.h
			mulh64.test.h
			mulh64.test.cpp
			)
	target_link_libraries(mulh64_test PRIVATE ${QtLib}::Test)
	add_test(NAME mulh64
			COMMAND mulh64_test)
endif()

