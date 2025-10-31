#include "GrammarToNfaConverter.h"
#include "RightLinearConverter.h"

#include <stdexcept>

namespace
{

SymbolString ReverseSymbolString(const SymbolString& symbols)
{
	if (symbols.empty() || symbols == std::to_string(EPSILON))
	{
		return symbols;
	}

	SymbolString reversed = symbols;
	std::reverse(reversed.begin(), reversed.end());
	return reversed;
}

Grammar ReverseGrammar(const Grammar& grammar)
{
	Grammar gPrime = grammar;
	gPrime.SetName(grammar.GetName() + "Reversed");

	const auto& nonTerminals = gPrime.GetNonTerminals();
	auto originalProductions = gPrime.GetProductions();

	gPrime.ClearProductions();

	for (const auto& prod : originalProductions)
	{
		Production reversedProd;
		reversedProd.m_left = prod.m_left;

		const SymbolString& rhs = prod.m_right;

		if (rhs.empty() || rhs == std::to_string(EPSILON))
		{
			reversedProd.m_right = std::to_string(EPSILON);
		}
		else
		{
			SymbolString firstSymbol = rhs.substr(0, 1);
			if (nonTerminals.contains(firstSymbol))
			{
				// Форма A -> Bw. Преобразуем в A -> w^R B
				SymbolString terminals = rhs.substr(1);
				reversedProd.m_right = ReverseSymbolString(terminals) + firstSymbol;
			}
			else
			{
				// Форма A -> w. Преобразуем в A -> w^R
				reversedProd.m_right = ReverseSymbolString(rhs);
			}
		}

		gPrime.AddProduction(reversedProd);
	}

	return gPrime;
}

Automaton ReverseAutomaton(const Automaton& mPrime, const Grammar& originalGrammar)
{
	Automaton m;
	m.SetTitle(originalGrammar.GetName() + "NFAL");

	State newStartState = 0;
	for (const State& s : mPrime.GetStates())
	{
		if (s >= newStartState)
		{
			newStartState = s + 1;
		}
	}

	m.SetStartState(newStartState);
	m.AddFinalState(mPrime.GetStartState());

	for (const State& finalState : mPrime.GetFinalStates())
	{
		m.AddTransition(newStartState, EPSILON, finalState);
	}

	for (const auto& [from, edges] : mPrime.GetTransitions())
	{
		for (const auto& [symbol, toSet] : edges)
		{
			for (const State& to : toSet)
			{
				m.AddTransition(to, symbol, from);
			}
		}
	}

	return m;
}
}

Automaton GrammarToNfaConverter::Convert(const Grammar& grammar)
{
	RegularGrammarType type = Grammar::GetLinearityType(grammar);

	if (type == RegularGrammarType::RIGHT_LINEAR)
	{
		return ConvertRightLinear(grammar);
	}

	if (type == RegularGrammarType::LEFT_LINEAR)
	{
		return ConvertLeftLinear(grammar);
	}

	throw std::invalid_argument("Grammar is not regular (mixed or non-linear)");
}

Automaton GrammarToNfaConverter::ConvertRightLinear(const Grammar& grammar)
{
	RightLinearConverter converter(grammar);
	return converter.Run();
}

Automaton GrammarToNfaConverter::ConvertLeftLinear(const Grammar& grammar)
{
	Grammar gPrime = ReverseGrammar(grammar);
	Automaton mPrime = ConvertRightLinear(gPrime);
	Automaton m = ReverseAutomaton(mPrime, grammar);
	return m;
}
