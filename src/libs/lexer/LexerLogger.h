#pragma once
#include <string>

class LexerLogger
{
public:
	static void Log(int pos, const std::string& lexeme, int tokenKind);
};
