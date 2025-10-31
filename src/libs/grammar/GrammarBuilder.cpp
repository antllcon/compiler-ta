#include "GrammarBuilder.h"
#include "Grammar.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

const std::string WHITESPACE = " \t\n\r\f\v";

void AssertIsFileOpen(const std::ifstream& file, const std::string& filename)
{
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + filename);
	}
}

void AssertRuleFormat(std::size_t arrowPos, const std::string& line)
{
	if (arrowPos == std::string::npos)
	{
		throw std::runtime_error("Invalid grammar rule format (missing '->'): " + line);
	}
}

void AssertLhsNotEmpty(const std::string& lhs, const std::string& line)
{
	if (lhs.empty())
	{
		throw std::runtime_error("Missing LHS in rule: " + line);
	}
}

void AssertRhsNotEmpty(const std::vector<std::string>& alternatives, const std::string& line)
{
	if (alternatives.empty())
	{
		throw std::runtime_error("Missing RHS in rule: " + line);
	}
}

std::string Trim(const std::string& str)
{
	size_t first = str.find_first_not_of(WHITESPACE);
	if (std::string::npos == first)
	{
		return "";
	}
	size_t last = str.find_last_not_of(WHITESPACE);
	return str.substr(first, (last - first + 1));
}

std::vector<std::string> Split(const std::string& str, char delimiter)
{
	std::vector<std::string> parts;
	std::stringstream ss(str);
	std::string part;

	while (std::getline(ss, part, delimiter))
	{
		std::string trimmedPart = Trim(part);
		if (!trimmedPart.empty())
		{
			parts.push_back(trimmedPart);
		}
	}

	return parts;
}

} // namespace

Grammar GrammarBuilder::FromFile(const std::string& filename)
{
	Grammar grammar;
	std::ifstream file(filename);
	AssertIsFileOpen(file, filename);

	std::string line;
	while (std::getline(file, line))
	{
		std::string trimmedLine = Trim(line);
		if (!trimmedLine.empty())
		{
			ParseLine(grammar, trimmedLine);
		}
	}

	DeduceTerminals(grammar);
	return grammar;
}

void GrammarBuilder::ParseLine(Grammar& grammar, const std::string& line)
{
	std::size_t arrowPos = line.find("->");
	AssertRuleFormat(arrowPos, line);

	std::string lhs = Trim(line.substr(0, arrowPos));
	std::string rhsFull = Trim(line.substr(arrowPos + 2));

	AssertLhsNotEmpty(lhs, line);

	grammar.AddNonTerminal(lhs);

	if (grammar.GetStartSymbol().empty())
	{
		grammar.SetStartSymbol(lhs);
	}

	auto alternatives = Split(rhsFull, '|');
	AssertRhsNotEmpty(alternatives, line);

	for (const auto& alt : alternatives)
	{
		grammar.AddProduction({ .m_left = lhs, .m_right = alt });
	}
}

void GrammarBuilder::DeduceTerminals(Grammar& grammar)
{
	const auto& nonTerminals = grammar.GetNonTerminals();
	for (const auto& prod : grammar.GetProductions())
	{
		for (char c : prod.m_right)
		{
			SymbolString symbol(1, c);
			if (!nonTerminals.contains(symbol))
			{
				grammar.AddTerminal(symbol);
			}
		}
	}
}