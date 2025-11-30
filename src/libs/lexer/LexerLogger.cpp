#include "LexerLogger.h"
#include <iostream>

void LexerLogger::Log(int pos, const std::string& lexeme, int tokenKind)
{
	std::cout << "(" << pos << ") - '" << lexeme << "' (" << tokenKind << ")" << std::endl;
}
