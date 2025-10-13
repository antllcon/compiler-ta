set(MODULES
        automaton
)

add_executable(minimize main.cpp)

foreach (MODULE ${MODULES})
    add_subdirectory(libs/${MODULE})
endforeach ()

target_link_libraries(minimize PRIVATE ${MODULES})