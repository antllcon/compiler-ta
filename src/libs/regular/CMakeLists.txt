add_library(regular RegexConverterAdapter.cpp AutomatonToRegexConverter.cpp nfa.c)
target_include_directories(regular PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})