add_library(lexer Lexer.cpp LexerLogger.cpp CodeRecognizer.cpp)
target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})