add_executable(regular_tests Regular.test.cpp)

target_link_libraries(regular_tests PRIVATE regular automaton GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(regular_tests)