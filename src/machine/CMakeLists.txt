project(machine
		DESCRIPTION "The actual simulator as a library. Link with an UI of your choice.")

set(CMAKE_AUTOMOC ON)

set(machine_SOURCES
		execute/alu.cpp
		csr/controlstate.cpp
		core.cpp
		instruction.cpp
		machine.cpp
		machineconfig.cpp
		memory/backend/lcddisplay.cpp
		memory/backend/memory.cpp
		memory/backend/peripheral.cpp
		memory/backend/peripspiled.cpp
		memory/backend/serialport.cpp
		memory/backend/aclintmtimer.cpp
		memory/backend/aclintmswi.cpp
		memory/backend/aclintsswi.cpp
		memory/cache/cache.cpp
		memory/cache/cache_policy.cpp
		memory/frontend_memory.cpp
		memory/memory_bus.cpp
		programloader.cpp
		predictor.cpp
		registers.cpp
		simulator_exception.cpp
		symboltable.cpp
		)

set(machine_HEADERS
		execute/alu.h
		csr/controlstate.h
		core.h
		core/core_state.h
		csr/address.h
		instruction.h
		machine.h
		machineconfig.h
		config_isa.h
		machinedefs.h
		memory/address.h
		memory/address_range.h
		memory/backend/backend_memory.h
		memory/backend/lcddisplay.h
		memory/backend/memory.h
		memory/backend/peripheral.h
		memory/backend/peripspiled.h
		memory/backend/serialport.h
		memory/backend/aclintmtimer.h
		memory/backend/aclintmswi.h
		memory/backend/aclintsswi.h
		memory/cache/cache.h
		memory/cache/cache_policy.h
		memory/cache/cache_types.h
		memory/frontend_memory.h
		memory/memory_bus.h
		memory/memory_utils.h
		programloader.h
		predictor_types.h
		predictor.h
		pipeline.h
		registers.h
		register_value.h
		simulator_exception.h
		symboltable.h
		utils.h
		execute/alu_op.h
		execute/mul_op.h
		)

# Object library is preferred, because the library archive is never really
# needed. This option skips the archive creation and links directly .o files.
add_library(machine STATIC
		${machine_SOURCES}
		${machine_HEADERS})
target_link_libraries(machine
		PRIVATE ${QtLib}::Core
		PUBLIC libelf)

if(NOT ${WASM})
	# Machine tests (not available on WASM)

	add_executable(alu_test
			execute/alu.test.cpp
			execute/alu.test.h
			execute/alu.cpp
			execute/alu.h
			)
	target_link_libraries(alu_test
			PRIVATE ${QtLib}::Core ${QtLib}::Test)
	add_test(NAME alu COMMAND alu_test)

	add_executable(registers_test
			register_value.h
			registers.cpp
			registers.h
			registers.test.cpp
			registers.test.h
			simulator_exception.cpp
			simulator_exception.h
			)
	target_link_libraries(registers_test
			PRIVATE ${QtLib}::Core ${QtLib}::Test)
	add_test(NAME registers COMMAND registers_test)

	add_executable(memory_test
			memory/backend/backend_memory.h
			memory/backend/memory.cpp
			memory/backend/memory.h
			memory/backend/memory.test.cpp
			memory/backend/memory.test.h
			memory/frontend_memory.cpp
			memory/frontend_memory.h
			memory/memory_bus.cpp
			memory/memory_bus.h
			simulator_exception.cpp
			simulator_exception.h
			tests/utils/integer_decomposition.h
			)
	target_link_libraries(memory_test
			PRIVATE ${QtLib}::Core ${QtLib}::Test)
	add_test(NAME memory COMMAND memory_test)

	add_executable(cache_test
			machineconfig.cpp
			machineconfig.h
			config_isa.h
			memory/backend/backend_memory.h
			memory/backend/memory.cpp
			memory/backend/memory.h
			memory/cache/cache.cpp
			memory/cache/cache.h
			memory/cache/cache.test.cpp
			memory/cache/cache.test.h
			memory/cache/cache_policy.cpp
			memory/cache/cache_policy.h
			memory/frontend_memory.cpp
			memory/frontend_memory.h
			memory/memory_bus.cpp
			memory/memory_bus.h
			simulator_exception.cpp
			simulator_exception.h
			tests/data/cache_test_performance_data.h
			tests/utils/integer_decomposition.h
			)
	target_link_libraries(cache_test
			PRIVATE ${QtLib}::Core ${QtLib}::Test)
	add_test(NAME cache COMMAND cache_test)

	add_executable(instruction_test
			csr/controlstate.cpp
			csr/controlstate.h
			instruction.cpp
			instruction.h
			instruction.test.cpp
			instruction.test.h
			simulator_exception.cpp
			simulator_exception.h
			)
	target_link_libraries(instruction_test
			PRIVATE ${QtLib}::Core ${QtLib}::Test)
	add_test(NAME instruction COMMAND instruction_test)

	add_executable(program_loader_test
			csr/controlstate.cpp
			csr/controlstate.h
			instruction.cpp
			instruction.h
			memory/backend/backend_memory.h
			memory/backend/memory.cpp
			memory/backend/memory.h
			programloader.cpp
			programloader.h
			programloader.test.cpp
			programloader.test.h
			simulator_exception.cpp
			simulator_exception.h
			symboltable.cpp
			symboltable.h
			)
	target_link_libraries(program_loader_test
			PRIVATE ${QtLib}::Core ${QtLib}::Test libelf)
	add_test(NAME program_loader COMMAND program_loader_test)


	add_executable(core_test
			csr/controlstate.cpp
			csr/controlstate.h
			core.cpp
			core.h
			core.test.cpp
			core.test.h
			execute/alu.cpp
			execute/alu.h
			instruction.cpp
			instruction.h
			memory/backend/backend_memory.h
			memory/backend/memory.cpp
			memory/backend/memory.h
			memory/cache/cache.cpp
			memory/cache/cache.h
			memory/cache/cache_policy.cpp
			memory/cache/cache_policy.h
			memory/frontend_memory.cpp
			memory/frontend_memory.h
			memory/memory_bus.cpp
			memory/memory_bus.h
			registers.cpp
			registers.h
			predictor.cpp
			predictor.h
			predictor_types.h
			simulator_exception.cpp
			simulator_exception.h
			machineconfig.cpp
			)
	target_link_libraries(core_test
			PRIVATE ${QtLib}::Core ${QtLib}::Test libelf)
	add_test(NAME core COMMAND core_test)

	add_custom_target(machine_unit_tests
			DEPENDS alu_test registers_test memory_test cache_test instruction_test program_loader_test core_test)
endif()
