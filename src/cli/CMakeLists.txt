project(cli
        LANGUAGES C CXX
        VERSION ${MAIN_PROJECT_VERSION}
        DESCRIPTION "Simulator command-line UI.")

set(CMAKE_AUTOMOC ON)

set(cli_SOURCES
    chariohandler.cpp
    main.cpp
    msgreport.cpp
    reporter.cpp
    tracer.cpp
    )
set(cli_HEADERS
    chariohandler.h
    msgreport.h
    reporter.h
    tracer.h
    )

add_executable(cli
               ${cli_SOURCES}
               ${cli_HEADERS})
target_link_libraries(cli
                      PRIVATE Qt5::Core machine assembler)
target_compile_definitions(cli
                           PRIVATE
                           APP_ORGANIZATION=\"${MAIN_PROJECT_ORGANIZATION}\"
                           APP_ORGANIZATION_DOMAIN=\"${MAIN_PROJECT_HOMEPAGE_URL}\"
                           APP_NAME=\"${MAIN_PROJECT_NAME}\"
                           APP_VERSION=\"${PROJECT_VERSION}\"
                           ENV_CONFIG_FILE_NAME=\"${MAIN_PROJECT_NAME_UPPER}_CONFIG_FILE\")
set_target_properties(cli PROPERTIES
                      OUTPUT_NAME "${MAIN_PROJECT_NAME_LOWER}_${PROJECT_NAME}")

