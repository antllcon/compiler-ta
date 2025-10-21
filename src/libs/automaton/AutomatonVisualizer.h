#pragma once

#include "Automaton.h"
#include <string>

class AutomatonVisualizer
{
public:
	using DfaStateKey = std::set<State>;
	using DfaTransitionTable = std::map<DfaStateKey, std::map<Symbol, DfaStateKey>>;

    static void Display(const Automaton& automaton);
    static void ExportToDot(const Automaton& automaton, const std::string& filename);
	static void PrintMinimizationTable(
		const std::vector<Symbol>& alphabet,
		const std::vector<std::set<State>>& partitions,
		const std::map<State, std::vector<int>>& stateSignatures,
		int iterationNumber
	);
	static void PrintDeterminizationTable(
		const std::vector<Symbol>& alphabet,
		const DfaTransitionTable& dfaTransitions);
};