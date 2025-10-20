#include "MinimizationAlghoritm.h"

#include "AutomatonVisualizer.h"

#include <iostream>
#include <queue>

namespace
{
constexpr int EMPTY_PARTITION = -1;

std::set<State> FindReachableStates(const Automaton& automaton)
{
	std::set<State> reachable;
	if (automaton.GetStates().empty())
	{
		return reachable;
	}

	std::queue<State> queue;
	const State startState = automaton.GetStartState();

	reachable.insert(startState);
	queue.push(startState);

	while (!queue.empty())
	{
		State currentState = queue.front();
		queue.pop();

		if (automaton.GetTransitions().contains(currentState))
		{
			for (const auto& onPair : automaton.GetTransitions().at(currentState))
			{
				for (State toState : onPair.second)
				{
					if (!reachable.contains(toState))
					{
						reachable.insert(toState);
						queue.push(toState);
					}
				}
			}
		}
	}
	return reachable;
}

std::vector<std::set<State>> InitialPartition(const Automaton& automaton, const std::set<State>& reachableStates)
{
	std::vector<std::set<State>> partitions;
	std::set<State> finalReachable;
	std::set<State> nonFinalReachable;

	for (const State state : reachableStates)
	{
		if (automaton.GetFinalStates().contains(state))
		{
			finalReachable.insert(state);
		}
		else
		{
			nonFinalReachable.insert(state);
		}
	}

	if (!finalReachable.empty())
	{
		partitions.emplace_back(finalReachable);
	}
	if (!nonFinalReachable.empty())
	{
		partitions.emplace_back(nonFinalReachable);
	}

	return partitions;
}

Automaton BuildMinimizedAutomaton(const Automaton& original, const std::vector<std::set<State>>& partitions)
{
	Automaton minimized;
	minimized.SetTitle(original.GetTitle() + "Minimized");
	if (partitions.empty())
	{
		return minimized;
	}

	std::map<State, State> oldStateToNewState;
	for (State newStateId = 0; newStateId < partitions.size(); ++newStateId)
	{
		for (const State oldState : partitions[newStateId])
		{
			oldStateToNewState[oldState] = newStateId;
		}
	}

	minimized.SetStartState(oldStateToNewState.at(original.GetStartState()));

	for (const State oldFinalState : original.GetFinalStates())
	{
		if (oldStateToNewState.contains(oldFinalState))
		{
			minimized.AddFinalState(oldStateToNewState.at(oldFinalState));
		}
	}

	for (State newStateId = 0; newStateId < partitions.size(); ++newStateId)
	{
		State representative = *partitions[newStateId].begin();
		if (original.GetTransitions().contains(representative))
		{
			for (const auto& onPair : original.GetTransitions().at(representative))
			{
				const Symbol symbol = onPair.first;
				const State oldDestState = *onPair.second.begin();
				if (oldStateToNewState.contains(oldDestState))
				{
					State newDestState = oldStateToNewState.at(oldDestState);
					minimized.AddTransition(newStateId, symbol, newDestState);
				}
			}
		}
	}

	return minimized;
}
} // namespace

Automaton MinimizationAlgorithm::Minimize(const Automaton& automaton, bool logSteps)
{
	if (!automaton.IsDeterministic())
	{
		throw std::logic_error("Minimization is only possible for a DFA");
	}

	if (automaton.GetStates().empty())
	{
		return automaton;
	}

	const auto reachableStates = FindReachableStates(automaton);
	auto partitions = InitialPartition(automaton, reachableStates);

	int iteration = 0;
	while (true)
	{
		iteration++;
		if (logSteps)
		{
			std::cout << "\nIteration " << iteration << std::endl;
		}

		if (!RefineSinglePass(automaton, partitions, logSteps, iteration))
		{
			if (logSteps) std::cout << "Partitions are stable. Minimization complete." << std::endl;
			break;
		}
	}

	return BuildMinimizedAutomaton(automaton, partitions);
}

bool MinimizationAlgorithm::RefineSinglePass(
	const Automaton& automaton,
	std::vector<std::set<State>>& partitions,
	bool logSteps,
	int iterationNumber)
{
	// Карта для быстрого поиска:
	// Старое состояние -> номер его класса эквивалентности
	std::map<State, int> stateToPartitionId;
	for (int i = 0; i < partitions.size(); ++i)
	{
		for (State state : partitions[i])
		{
			stateToPartitionId[state] = i;
		}
	}

	// Вектор номеров классов эквивалентности,
	// куда ведут переходы по каждому символу алфавита
	std::map<State, std::vector<int>> stateSignatures;

	// Сортированный алфавит
	const std::vector<Symbol> alphabet(automaton.GetAlphabet().begin(), automaton.GetAlphabet().end());
	const auto& transitions = automaton.GetTransitions();

	for (const State state : automaton.GetStates())
	{
		std::vector<int> signature;
		for (const Symbol symbol : alphabet)
		{
			int destPartitionId = EMPTY_PARTITION;

			if (transitions.contains(state) && transitions.at(state).contains(symbol))
			{
				const State destState = *transitions.at(state).at(symbol).begin();
				if (stateToPartitionId.contains(destState))
				{
					destPartitionId = stateToPartitionId.at(destState);
				}
			}

			signature.emplace_back(destPartitionId);
		}
		stateSignatures[state] = signature;
	}

	if (logSteps)
	{
		AutomatonVisualizer::PrintMinimizationTable(alphabet, partitions, stateSignatures, iterationNumber);
	}

	std::vector<std::set<State>> newPartitions;
	for (const auto& partition : partitions)
	{
		// Группируем состояния внутри одного старого класса эквивалентности по их наобрам переходов по алфавиту
		// Ключ - набор переходов по алфавиту, Значение - множество состояний с этим ключом
		std::map<std::vector<int>, std::set<State>> groups;
		for (const State state : partition)
		{
			if (stateSignatures.contains(state))
			{
				groups[stateSignatures.at(state)].insert(state);
			}
		}

		for (const auto& groupPair : groups)
		{
			newPartitions.emplace_back(groupPair.second);
		}
	}

	if (newPartitions.size() > partitions.size())
	{
		partitions = std::move(newPartitions);
		return true;
	}

	return false;
}
