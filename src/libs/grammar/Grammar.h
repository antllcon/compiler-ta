#pragma once

#include <set>
#include <string>
#include <vector>

using SymbolString = std::string; // Терминалы и нетерминалы как строки

enum class ChomskyType
{
	TURING = 0,			  // Фразовые
	CONTEXT_DEPENDED = 1, // Контекстно-зависимые
	CONTEXT_FREE = 2,	  // Контекстно-свободные
	REGULAR = 3,		  // Регулярные
	UNKNOWN = -1		  // Если тип еще не определен
};

enum class RegularGrammarType
{
	LEFT_LINEAR,  // Все правила вида A -> Bw или A -> w
	RIGHT_LINEAR, // Все правила вида A -> wB или A -> w
	UNDEFINED	  // Смешанный тип, или нелинейная, или не-регулярная
};

// Правило вывода
struct Production
{
	SymbolString m_left;
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

	void AddProduction(Production production);
	const std::vector<Production>& GetProductions() const;

	bool IsRegular() const;
	void ClearProductions();
	static RegularGrammarType GetLinearityType(const Grammar& grammar);

private:
	std::string m_name;
	ChomskyType m_type = ChomskyType::UNKNOWN;
	std::set<SymbolString> m_terminals;
	std::set<SymbolString> m_nonTerminals;
	SymbolString m_startSymbol;
	std::vector<Production> m_productions;
};