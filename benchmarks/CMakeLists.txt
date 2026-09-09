add_executable(qtrvsim_component_benchmark
        component_benchmark.cpp)
target_link_libraries(qtrvsim_component_benchmark
        PRIVATE ${QtLib}::Core machine)
target_compile_definitions(qtrvsim_component_benchmark
        PRIVATE
        QTRVSIM_BENCHMARK_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        QTRVSIM_BENCHMARK_COMPILER="${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

if (TARGET gui_lib)
    add_executable(qtrvsim_gui_benchmark
            gui_benchmark.cpp)
    target_link_libraries(qtrvsim_gui_benchmark
            PRIVATE gui_lib machine
            ${QtLib}::Core ${QtLib}::Gui ${QtLib}::Widgets)
    target_compile_definitions(qtrvsim_gui_benchmark
            PRIVATE
            QTRVSIM_BENCHMARK_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
            QTRVSIM_BENCHMARK_COMPILER="${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

    add_test(
            NAME gui_benchmark_smoke
            COMMAND qtrvsim_gui_benchmark
            --scenario cli-single-hash
            --scenario cli-pipeline-cache-memory
            --scenario cli-single-peripheral
            --scenario cli-pipeline-cache-mmu
            --scenario cli-pipeline-cache-predictor-classifier
            --scenario gui-single-hash-core-max
            --scenario gui-pipeline-cache-memory-caches-unlimited
            --scenario gui-pipeline-cache-peripheral-peripherals-unlimited
            --scenario gui-pipeline-cache-mmu-tlbs-unlimited
            --scenario gui-pipeline-cache-predictor-classifier-all-unlimited
            --samples 2
            --warmups 1
            --work-units 80)
    set_tests_properties(gui_benchmark_smoke PROPERTIES
            ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif ()

add_test(
        NAME component_benchmark_smoke
        COMMAND qtrvsim_component_benchmark
        --scenario core-single
        --samples 2
        --warmups 0
        --work-units 100)
