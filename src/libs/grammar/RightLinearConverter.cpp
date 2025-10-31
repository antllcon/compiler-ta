#include "RightLinearConverter.h"

RightLinearConverter::RightLinearConverter(const Grammar& grammar)
	: m_grammar(grammar)
	, m_finalState(0)
	, m_nextStateId(0)
{
}

Automaton RightLinearConverter::Run()
{
	m_automaton.SetTitle(m_grammar.GetName() + "NFAR");

	InitializeStates();
	SetStartAndFinalStates();
	ProcessProductions();

	return m_automaton;
}

void RightLinearConverter::InitializeStates()
{
	for (const auto& nonTerminal : m_grammar.GetNonTerminals())
	{
		m_stateMap[nonTerminal] = GetNextStateId();
	}

	m_finalState = GetNextStateId();
}

void RightLinearConverter::SetStartAndFinalStates()
{
	State startState = m_stateMap.at(m_grammar.GetStartSymbol());
	m_automaton.SetStartState(startState);

	m_automaton.AddFinalState(m_finalState);

	for (const auto& prod : m_grammar.GetProductions())
	{
		if (prod.m_left == m_grammar.GetStartSymbol() && IsEpsilonRule(prod))
		{
			m_automaton.AddFinalState(startState);
			break;
		}
	}
}

void RightLinearConverter::ProcessProductions()
{
	for (const auto& prod : m_grammar.GetProductions())
	{
		ProcessProduction(prod);
	}
}

void RightLinearConverter::ProcessProduction(const Production& prod)
{
	if (IsEpsilonRule(prod))
	{
		State currentState = m_stateMap.at(prod.m_left);
		m_automaton.AddTransition(currentState, EPSILON, m_finalState);
		return;
	}

	State currentState = m_stateMap.at(prod.m_left);
	const SymbolString& rhs = prod.m_right;

	State targetState;
	SymbolString terminalPrefix;

	const auto& nonTerminals = m_grammar.GetNonTerminals();
	SymbolString lastSymbol;

	if (!rhs.empty())
	{
		lastSymbol = rhs.substr(rhs.length() - 1, 1);
	}

	// Определяем тип правила: A -> wB или A -> w
	if (!rhs.empty() && nonTerminals.contains(lastSymbol))
	{
		// Тип A -> wB (e.g., A -> abC)
		targetState = m_stateMap.at(lastSymbol);
		terminalPrefix = rhs.substr(0, rhs.length() - 1);
	}
	else
	{
		// Тип A -> w (e.g., A -> ab)
		targetState = m_finalState;
		terminalPrefix = rhs;
	}

	AddTransitionChain(currentState, terminalPrefix, targetState);
}

void RightLinearConverter::AddTransitionChain(State from, const SymbolString& terminals, State to)
{
	State current = from;

	if (terminals.empty())
	{
		m_automaton.AddTransition(current, EPSILON, to);
		return;
	}

	for (size_t i = 0; i < terminals.length(); ++i)
	{
		Symbol sym = terminals[i];
		if (i == terminals.length() - 1)
		{
			m_automaton.AddTransition(current, sym, to);
		}
		else
		{
			State intermediateState = GetNextStateId();
			m_automaton.AddTransition(current, sym, intermediateState);
			current = intermediateState;
		}
	}
}

bool RightLinearConverter::IsEpsilonRule(const Production& prod)
{
	return prod.m_right.length() == 1 && prod.m_right[0] == EPSILON;
}

State RightLinearConverter::GetNextStateId() noexcept
{
	return m_nextStateId++;
}
