set(MODULES automaton grammar)

foreach (MODULE ${MODULES})
    add_subdirectory(libs/${MODULE})
endforeach ()

add_executable(compiler-ta main.cpp)

target_link_libraries(compiler-ta PRIVATE ${MODULES})

set_target_properties(compiler-ta PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/res"
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()