#pragma once

#include "Automaton.h"
#include <vector>

class MinimizationAlgorithm
{
public:
	MinimizationAlgorithm() = default;
	virtual ~MinimizationAlgorithm() = default;
	static Automaton Minimize(const Automaton& automaton, bool logSteps = false);

private:
	static bool RefineSinglePass(
		const Automaton& automaton,
		std::vector<std::set<State>>& partitions,
		bool logSteps,
		int iterationNumber);
};