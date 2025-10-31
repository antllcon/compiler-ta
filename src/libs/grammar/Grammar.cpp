#include "Grammar.h"
#include <stdexcept>

namespace
{
void AssertIsTSymbolStringExist(const SymbolString& symbol)
{
	if (symbol.empty())
	{
		throw std::invalid_argument("Empty symbol string");
	}
}

void AssertIsSymbolNonTerminal(const SymbolString& symbol, const std::set<SymbolString>& nonTerminals)
{
	if (!nonTerminals.contains(symbol))
	{
		throw std::invalid_argument("Symbol '" + symbol + "' must be one of the known non-terminals");
	}
}
} // namespace

void Grammar::SetName(const std::string& name)
{
	m_name = name;
}

void Grammar::SetType(ChomskyType type)
{
	m_type = type;
}

void Grammar::AddTerminal(const SymbolString& terminal)
{
	AssertIsTSymbolStringExist(terminal);
	m_terminals.insert(terminal);
}

void Grammar::AddNonTerminal(const SymbolString& nonTerminal)
{
	AssertIsTSymbolStringExist(nonTerminal);
	m_nonTerminals.insert(nonTerminal);
}

// Делать проверку после добавления терминальных символов
void Grammar::SetStartSymbol(const SymbolString& startSymbol)
{
	AssertIsSymbolNonTerminal(startSymbol, m_nonTerminals);
	m_startSymbol = startSymbol;
}

// Правая часть правила не должна быть пустой (ε-правила обрабатываются особо)
// Замечание: Классические ε-правила имеют пустую правую часть
void Grammar::AddProduction(Production production)
{
	AssertIsTSymbolStringExist(production.m_left);
	AssertIsTSymbolStringExist(production.m_right);

	// - Проверить, что все символы в m_left существуют в m_nonTerminals или m_terminals
	// - Проверить, что все символы в m_right существуют в m_nonTerminals или m_terminals
	m_productions.push_back(std::move(production));
}

const std::string& Grammar::GetName() const
{
	return m_name;
}

ChomskyType Grammar::GetType() const
{
	// TODO: Если m_type == UNKNOWN, можно добавить логику для автоматического определения типа
	// на основе анализа m_productions (но это сложная задача)
	return m_type;
}

const std::set<SymbolString>& Grammar::GetTerminals() const
{
	return m_terminals;
}

const std::set<SymbolString>& Grammar::GetNonTerminals() const
{
	return m_nonTerminals;
}

const SymbolString& Grammar::GetStartSymbol() const
{
	return m_startSymbol;
}

const std::vector<Production>& Grammar::GetProductions() const
{
	return m_productions;
}

bool Grammar::IsRegular() const
{
	return GetLinearityType(*this) != RegularGrammarType::UNDEFINED;
}

void Grammar::ClearProductions()
{
	m_productions.clear();
}

RegularGrammarType Grammar::GetLinearityType(const Grammar& grammar)
{
	std::set<char> nonTerminalChars;
	for (const auto& ntString : grammar.GetNonTerminals())
	{
		if (ntString.length() != 1)
		{
			return RegularGrammarType::UNDEFINED;
		}
		nonTerminalChars.insert(ntString[0]);
	}

	bool hasLeftLinearRule = false;
	bool hasRightLinearRule = false;

	for (const auto& prod : grammar.GetProductions())
	{
		if (prod.m_left.length() != 1 || nonTerminalChars.count(prod.m_left[0]) == 0)
		{
			return RegularGrammarType::UNDEFINED;
		}

		const std::string& rhs = prod.m_right;

		if (rhs.empty())
		{
			continue;
		}

		// Анализируем правую часть (RHS)
		int nonTerminalCount = 0;
		int firstNonTerminalPos = -1;
		int lastNonTerminalPos = -1;

		for (int i = 0; i < rhs.length(); ++i)
		{
			if (nonTerminalChars.contains(rhs[i]))
			{
				nonTerminalCount++;
				lastNonTerminalPos = i;
				if (firstNonTerminalPos == -1)
				{
					firstNonTerminalPos = i;
				}
			}
		}

		// Принимаем решение на основе анализа RHS
		if (nonTerminalCount == 0)
		{
			continue;
		}
		else if (nonTerminalCount == 1)
		{
			// Нетерминал стоит в самом начале
			if (firstNonTerminalPos == 0)
			{
				hasLeftLinearRule = true;
			}
			// Нетерминал стоит в самом конце
			else if (lastNonTerminalPos == rhs.length() - 1)
			{
				hasRightLinearRule = true;
			}
			// Форма: A -> wBv (e.g., "aBa")
			else
			{
				// Нетерминал в середине. Это нерегулярная грамматика
				return RegularGrammarType::UNDEFINED;
			}
		}
		else
		{
			return RegularGrammarType::UNDEFINED;
		}
	}

	if (hasLeftLinearRule && hasRightLinearRule)
	{
		return RegularGrammarType::UNDEFINED;
	}
	else if (hasLeftLinearRule)
	{
		return RegularGrammarType::LEFT_LINEAR;
	}
	else if (hasRightLinearRule)
	{
		return RegularGrammarType::RIGHT_LINEAR;
	}
	else
	{
		return RegularGrammarType::RIGHT_LINEAR;
	}
}