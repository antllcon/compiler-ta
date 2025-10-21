#pragma once

#include "Automaton.h"
#include <set>

class DeterminizationAlgorithm
{
public:
	DeterminizationAlgorithm() = default;
	~DeterminizationAlgorithm() = default;

	static Automaton Determine(const Automaton& nfa, bool logSteps = false);
	static std::set<State> EpsilonClosure(const Automaton& nfa, State state);
	static std::set<State> EpsilonClosure(const Automaton& nfa, const std::set<State>& states);
	static std::set<State> Move(const Automaton& nfa, const std::set<State>& states, Symbol symbol);
};