#pragma once

#include <set>
#include <string>
#include <vector>

using SymbolString = std::string; // Терминалы и нетерминалы как строки

enum class ChomskyType
{
	TURING = 0,			  // Рекурсивно перечислимые (Turing machines)
	CONTEXT_DEPENDED = 1, // Контекстно-зависимые (Linear Bounded Automata)
	CONTEXT_FREE = 2,	  // Контекстно-свободные (Pushdown Automata)
	REGULAR = 3,		  // Регулярные (Finite Automata)
	UNKNOWN = -1		  // Если тип еще не определен
};

/**
 * @brief Структура для хранения одного правила вывода (продукции)
 */
struct Production
{
	// TODO: не забыть написать проверку в Assert
	// Левая часть правила (может быть > 1 символа для типов 0, 1)
	SymbolString m_left;
	// Правая часть правила
	SymbolString m_right;
};

class Grammar
{
public:
	Grammar() = default;
	~Grammar() = default;

	void SetName(const std::string& name);
	const std::string& GetName() const;

	void SetType(ChomskyType type);
	ChomskyType GetType() const;

	void AddTerminal(const SymbolString& terminal);
	const std::set<SymbolString>& GetTerminals() const;

	void AddNonTerminal(const SymbolString& nonTerminal);
	const std::set<SymbolString>& GetNonTerminals() const;

	void SetStartSymbol(const SymbolString& startSymbol);
	const SymbolString& GetStartSymbol() const;

	void AddProductionFromString(const std::string& productionString);
	void AddProduction(Production production);

	const std::vector<Production>& GetProductions() const;
	bool IsRegular() const;

	// TODO: добавить проверки (IsTuring()...) для вида грамматики

private:
	// Production ParseProductionString(const std::string& productionString) const; // Написать в namespace {} в cpp файле

	std::string m_name;
	ChomskyType m_type = ChomskyType::UNKNOWN;
	std::set<SymbolString> m_terminals;
	std::set<SymbolString> m_nonTerminals;
	SymbolString m_startSymbol;
	std::vector<Production> m_productions;
};