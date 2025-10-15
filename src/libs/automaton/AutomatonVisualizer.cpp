#include "AutomatonVisualizer.h"

#include <fstream>
#include <iostream>

namespace
{
void AssertIsFileOpen(const std::ofstream& file)
{
	if (!file.is_open())
	{
		throw std::invalid_argument("The file cannot be opened");
	}
}
} // namespace

void AutomatonVisualizer::Display(const Automaton& automaton)
{
	std::cout << "Automaton";
	if (!automaton.GetTitle().empty())
	{
		std::cout << " " << automaton.GetTitle();
	}
	std::cout << std::endl;

	std::cout << "States (Q):       " << "{ ";
	for (const State state : automaton.GetStates())
	{
		std::cout << state << " ";
	}
	std::cout << "}" << std::endl;

	std::cout << "Alphabet (Σ):     " << "{ ";
	for (const Symbol symbol : automaton.GetAlphabet())
	{
		std::cout << symbol << " ";
	}
	std::cout << "}" << std::endl;

	std::cout << "Start State (q₀): " << "{ " << automaton.GetStartState() << " }" << std::endl;

	std::cout << "Final States (F): " << "{ ";
	for (const State state : automaton.GetFinalStates())
	{
		std::cout << state << " ";
	}
	std::cout << "}" << std::endl;

	std::cout << "Transitions (δ):  " << "{" << std::endl;
	if (automaton.GetTransitions().empty())
	{
		std::cout << "(No transitions defined)" << std::endl;
	}
	else
	{
		for (const auto& fromPair : automaton.GetTransitions())
		{
			for (const auto& onPair : fromPair.second)
			{
				std::cout << "	δ(" << fromPair.first << ", ";
				if (onPair.first == EPSILON)
				{
					std::cout << "ε";
				}
				else
				{
					std::cout << onPair.first;
				}
				std::cout << ") -> { ";
				for (const State toState : onPair.second)
				{
					std::cout << toState << " ";
				}
				std::cout << "}" << std::endl;
			}
		}
	}
	std::cout << "}" << std::endl;
}

void AutomatonVisualizer::ExportToDot(const Automaton& automaton, const std::string& filename)
{
	std::ofstream file(filename);
	AssertIsFileOpen(file);

	file << "digraph " << (automaton.GetTitle().empty() ? filename : automaton.GetTitle()) << std::endl;
	file << " {" << std::endl;

	file << "    start = " << automaton.GetStartState() << ";" << std::endl;
	file << "    final = ";
	std::string finalStatesStr;
	for (const State state : automaton.GetFinalStates())
	{
		finalStatesStr += std::to_string(state) + ", ";
	}
	if (!finalStatesStr.empty())
	{
		finalStatesStr.resize(finalStatesStr.length() - 2);
	}
	file << finalStatesStr << ";" << std::endl
		 << std::endl;

	std::map<std::pair<State, State>, std::vector<Symbol>> transitionsByPair;
	for (const auto& fromPair : automaton.GetTransitions())
	{
		for (const auto& onPair : fromPair.second)
		{
			for (const State toState : onPair.second)
			{
				transitionsByPair[{fromPair.first, toState}].emplace_back(onPair.first);
			}
		}
	}

	for (const auto& pair : transitionsByPair)
	{
		const State from = pair.first.first;
		const State to = pair.first.second;
		const auto& symbols = pair.second;

		std::string labelStr;
		bool hasEpsilonTransition = false;

		for (const Symbol symbol : symbols)
		{
			if (symbol == EPSILON)
			{
				hasEpsilonTransition = true;
			}
			else
			{
				labelStr += std::string(1, symbol) + ", ";
			}
		}

		if (hasEpsilonTransition && !labelStr.empty())
		{
			std::string warningLabels = labelStr;
			warningLabels.resize(warningLabels.length() - 2);
			std::cerr << "[Info - " << filename << "]" << std::endl;
			std::cerr << "Transition " << from << " -> " << to << std::endl;
			std::cerr << "has both ε and symbols {" << warningLabels << "}" << std::endl;
		}

		if (!labelStr.empty())
		{
			labelStr.resize(labelStr.length() - 2);
			file << "    " << from << " -> " << to << " [label=\"" << labelStr << "\"];" << std::endl;
		}

		if (hasEpsilonTransition)
		{
			file << "    " << from << " -> " << to << ";" << std::endl;
		}
	}

	file << "}" << std::endl;
}