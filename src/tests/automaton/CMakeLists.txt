add_executable(automaton_tests
        Minimization.test.cpp
        Determinization.test.cpp)

target_link_libraries(automaton_tests PRIVATE automaton GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(automaton_tests)