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
		throw std::invalid_argument("Symbol '" + symbol + "' must be one of the known non-terminals.");
	}
}


Production ParseProductionStringImpl(const std::string& /*productionString*/)
{
	return {};
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

void Grammar::AddProductionFromString(const std::string& productionString)
{
	Production production = ParseProductionStringImpl(productionString);
	AddProduction(std::move(production));
}

bool Grammar::IsRegular() const
{
	if (m_productions.empty())
	{
		return true;
	}

	int determinedType = -1;
	for (const auto& production : m_productions)
	{
		// 1. Проверка левой части: Должен быть один нетерминал
		if (production.m_left.length() != 1 || !m_nonTerminals.contains(production.m_left))
		{
			// Нарушение основного правила для Типа 3
			return false;
		}

		// 2. Анализ правой части и определение типа правила
		auto currentRuleType = -1;
		bool ruleOk = false;

		const SymbolString& right = production.m_right;

		if (right.empty()) // Правило A -> ε
		{
			// ε-правила допустимы и подходят под оба типа
			ruleOk = true;
		}
		else if (right.length() == 1) // Правило A -> a
		{
			if (m_terminals.contains(right))
			{
				// Подходит под оба типа
				ruleOk = true;
			}
		}
		else if (right.length() == 2) // Правила A -> aB или A -> Ba
		{
			SymbolString firstSymbol = right.substr(0, 1);
			SymbolString secondSymbol = right.substr(1, 1);

			// Проверка на праволинейность (A -> aB)
			if (m_terminals.contains(firstSymbol) && m_nonTerminals.contains(secondSymbol))
			{
				ruleOk = true;
				currentRuleType = 0;
			}
			// Проверка на леволинейность (A -> Ba)
			else if (m_nonTerminals.contains(firstSymbol) && m_terminals.contains(secondSymbol))
			{
				ruleOk = true;
				currentRuleType = 1;
			}
		}

		if (!ruleOk)
		{
			return false;
		}

		// 3. Проверка на смешивание типов
		// Если текущее правило однозначно право- или леволинейное:
		if (currentRuleType != -1)
		{
			// Если тип грамматики еще не определен, устанавливаем его
			if (determinedType == -1)
			{
				determinedType = currentRuleType;
			}
			// Если тип уже определен и он не совпадает с текущим правилом - ошибка
			else if (determinedType != currentRuleType)
			{
				return false;
			}
		}
	}

	return true;
}
