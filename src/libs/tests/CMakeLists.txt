add_executable(run_tests Minimization.test.cpp)
target_link_libraries(run_tests PRIVATE automaton GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(run_tests)