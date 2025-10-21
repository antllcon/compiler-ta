#include "DeterminizationAlgorithm.h"
#include "AutomatonVisualizer.h"

#include <iostream>
#include <queue>

namespace
{
const std::string DETERMINIZED_SUFFIX = "Determinized";
} // namespace

Automaton DeterminizationAlgorithm::Determine(const Automaton& nfa, bool logSteps)
{
	Automaton dfa;
	dfa.SetTitle(nfa.GetTitle() + DETERMINIZED_SUFFIX);
	if (nfa.GetStates().empty())
	{
		return dfa;
	}

	std::map<std::set<State>, State> dfaStateRegister;
	std::queue<std::set<State>> unprocessedStates;
	AutomatonVisualizer::DfaTransitionTable dfaTransitionsForPrint;

	auto startStateKey = EpsilonClosure(nfa, nfa.GetStartState());

	State nextDfaStateId = 0;
	dfaStateRegister[startStateKey] = nextDfaStateId;
	unprocessedStates.push(startStateKey);
	dfa.SetStartState(nextDfaStateId);

	while (!unprocessedStates.empty())
	{
		auto currentStateKey = unprocessedStates.front();
		unprocessedStates.pop();
		const auto dfaState = dfaStateRegister.at(currentStateKey);

		// Обходим алфавит и ищем новые объединенные состояния
		for (const Symbol symbol : nfa.GetAlphabet())
		{
			auto nextStateKey = EpsilonClosure(nfa, Move(nfa, currentStateKey, symbol));
			dfaTransitionsForPrint[currentStateKey][symbol] = nextStateKey;

			if (nextStateKey.empty())
			{
				continue;
			}

			// Если нашли, то проверим, вдруг уже такое состояние есть
			if (!dfaStateRegister.contains(nextStateKey))
			{
				nextDfaStateId++;
				dfaStateRegister[nextStateKey] = nextDfaStateId;
				unprocessedStates.push(nextStateKey);
			}

			// Добавляем переход в новый ДКА
			const auto destinationDfaStateId = dfaStateRegister.at(nextStateKey);
			dfa.AddTransition(dfaState, symbol, destinationDfaStateId);
		}
	}

	if (logSteps)
	{
		std::cout << "Determinization Transition table" << std::endl;

		const auto& nfaAlphabet = nfa.GetAlphabet();
		std::vector sortedAlphabet(nfaAlphabet.begin(), nfaAlphabet.end());
		std::sort(sortedAlphabet.begin(), sortedAlphabet.end());

		AutomatonVisualizer::PrintDeterminizationTable(sortedAlphabet, dfaTransitionsForPrint);
	}

	// Определение конечных состояний
	const auto& nfaFinalStates = nfa.GetFinalStates();
	for (const auto& pair : dfaStateRegister)
	{
		const auto& nfaStates = pair.first;
		const auto dfaState = pair.second;

		for (const auto finalStateNfa : nfaFinalStates)
		{
			if (nfaStates.contains(finalStateNfa))
			{
				dfa.AddFinalState(dfaState);
				break;
			}
		}
	}

	return dfa;
}

std::set<State> DeterminizationAlgorithm::EpsilonClosure(const Automaton& nfa, State state)
{
	std::set<State> closure;
	std::queue<State> queue;

	closure.insert(state);
	queue.push(state);

	const auto& transitions = nfa.GetTransitions();
	while (!queue.empty())
	{
		State currentState = queue.front();
		queue.pop();

		if (!transitions.contains(currentState) || !transitions.at(currentState).contains(EPSILON))
		{
			continue;
		}

		for (const State targetState : transitions.at(currentState).at(EPSILON))
		{
			if (!closure.contains(targetState))
			{
				closure.insert(targetState);
				queue.push(targetState);
			}
		}
	}

	return closure;
}

std::set<State> DeterminizationAlgorithm::EpsilonClosure(const Automaton& nfa, const std::set<State>& states)
{
	std::set<State> totalClosure;
	for (const State state : states)
	{
		std::set<State> singleClosure = EpsilonClosure(nfa, state);
		totalClosure.insert(singleClosure.begin(), singleClosure.end());
	}

	return totalClosure;
}

std::set<State> DeterminizationAlgorithm::Move(const Automaton& nfa, const std::set<State>& states, Symbol symbol)
{
	std::set<State> result;
	const auto& transitions = nfa.GetTransitions();

	for (const State state : states)
	{
		if (transitions.contains(state) && transitions.at(state).contains(symbol))
		{
			const auto& destinationStates = transitions.at(state).at(symbol);
			result.insert(destinationStates.begin(), destinationStates.end());
		}
	}

	return result;
}