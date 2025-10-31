add_executable(grammar_tests Grammar.test.cpp)

target_link_libraries(grammar_tests PRIVATE grammar automaton GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(grammar_tests)