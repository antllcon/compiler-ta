#include "Automaton.h"

#include <regex>

namespace
{
void AssertIsAutomatonDeterministic(const Automaton& automaton)
{
	if (!automaton.IsDeterministic())
	{
		throw std::invalid_argument("Automaton is not deterministic");
	}
}
} // namespace

bool Automaton::IsDeterministic() const
{
	for (const auto& fromPair : m_transitions)
	{
		for (const auto& onPair : fromPair.second)
		{
			if (onPair.first == EPSILON)
			{
				return false;
			}

			if (onPair.second.size() > 1)
			{
				return false;
			}
		}
	}

	return true;
}

void Automaton::SetTitle(const std::string& title)
{
	m_title = title;
}

void Automaton::SetStartState(const State startState)
{
	m_startState = startState;
	m_states.insert(startState);
}

void Automaton::AddFinalState(const State finalState)
{
	m_finalStates.insert(finalState);
	m_states.insert(finalState);
}

void Automaton::AddTransition(const State from, const Symbol on, const State to)
{
	m_states.insert(from);
	m_states.insert(to);

	if (on != EPSILON)
	{
		m_alphabet.insert(on);
	}

	m_transitions[from][on].insert(to);
}

const std::string& Automaton::GetTitle() const
{
	return m_title;
}

const std::set<State>& Automaton::GetStates() const
{
	return m_states;
}

const std::set<Symbol>& Automaton::GetAlphabet() const
{
	return m_alphabet;
}

const std::map<State, std::map<Symbol, std::set<State>>>& Automaton::GetTransitions() const
{
	return m_transitions;
}

State Automaton::GetStartState() const
{
	return m_startState;
}

const std::set<State>& Automaton::GetFinalStates() const
{
	return m_finalStates;
}

void Automaton::Clear()
{
	m_title.clear();
	m_states.clear();
	m_alphabet.clear();
	m_transitions.clear();
	m_startState = 0;
	m_finalStates.clear();
}

void Automaton::Swap(Automaton& other)
{
	std::swap(m_title, other.m_title);
	std::swap(m_states, other.m_states);
	std::swap(m_alphabet, other.m_alphabet);
	std::swap(m_transitions, other.m_transitions);
	std::swap(m_startState, other.m_startState);
	std::swap(m_finalStates, other.m_finalStates);
}