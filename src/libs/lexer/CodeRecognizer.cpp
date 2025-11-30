#include "CodeRecognizer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

CodeRecognizer::CodeRecognizer(bool logSteps)
	: logSteps(logSteps)
{
}

std::string CodeRecognizer::ReadFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("Cannot open file: " + path);

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void CodeRecognizer::ProcessFile(const std::string& filePath)
{
	std::string text = ReadFile(filePath);

	lexer.SetText(text);

	while (true)
	{
		auto [kind, value, position] = lexer.Next();

		if (logSteps)
			logger.Log(position, value, static_cast<int>(kind));

		if (kind == TokenKind::EndOfFile)
		{
			break;
		}
	}
}
