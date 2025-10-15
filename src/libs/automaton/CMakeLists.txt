add_library(automaton Automaton.cpp AutomatonBuilder.cpp AutomatonVisualizer.cpp)
target_include_directories(automaton PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})