add_executable(qtrvsim_component_benchmark
        component_benchmark.cpp)
target_link_libraries(qtrvsim_component_benchmark
        PRIVATE ${QtLib}::Core machine)
target_compile_definitions(qtrvsim_component_benchmark
        PRIVATE
        QTRVSIM_BENCHMARK_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        QTRVSIM_BENCHMARK_COMPILER="${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

add_test(
        NAME component_benchmark_smoke
        COMMAND qtrvsim_component_benchmark
        --scenario core-single
        --samples 2
        --warmups 0
        --work-units 100)
