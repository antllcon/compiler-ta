add_library(grammar
        Grammar.cpp
        GrammarBuilder.cpp
        GrammarToNfaConverter.cpp
        RightLinearConverter.cpp
)
target_include_directories(grammar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})