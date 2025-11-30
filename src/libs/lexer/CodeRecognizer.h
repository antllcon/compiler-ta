#pragma once
#include <string>
#include "Lexer.h"
#include "LexerLogger.h"

class CodeRecognizer
{
public:
	explicit CodeRecognizer(bool logSteps = false);

	void ProcessFile(const std::string& filePath);

private:
	bool logSteps;
	Lexer lexer;
	LexerLogger logger;

	static std::string ReadFile(const std::string& path);
};
