#pragma once

#include <string>

class Grammar;

class GrammarBuilder
{
public:
	static Grammar FromFile(const std::string& filename);

private:
	static void ParseLine(Grammar& grammar, const std::string& line);
	static void DeduceTerminals(Grammar& grammar);
};