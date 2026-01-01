project(debuginfo
        DESCRIPTION "Debug information provider maping addresses to line numbers.")

add_library(debuginfo STATIC
        debuginfo.cpp
        debuginfo.h
)

target_include_directories(debuginfo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT ${WASM})
    set(CMAKE_AUTOMOC ON)

    add_executable(debuginfo_test
            debuginfo.test.cpp
            debuginfo.test.h
    )
    target_include_directories(debuginfo_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(debuginfo_test
            PRIVATE debuginfo ${QtLib}::Core ${QtLib}::Test)
    add_test(NAME debuginfo COMMAND debuginfo_test)
endif ()
