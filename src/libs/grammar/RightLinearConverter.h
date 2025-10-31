#pragma once
#include "Grammar.h"
#include "../automaton/Automaton.h"

class RightLinearConverter
{
public:
	explicit RightLinearConverter(const Grammar& grammar);
	Automaton Run();

private:
	void InitializeStates();
	void SetStartAndFinalStates();
	void ProcessProductions();
	void ProcessProduction(const Production& prod);
	void AddTransitionChain(State from, const SymbolString& terminals, State to);
	static bool IsEpsilonRule(const Production& prod);
	State GetNextStateId() noexcept;

	const Grammar& m_grammar;
	Automaton m_automaton;
	std::map<SymbolString, State> m_stateMap;
	State m_finalState;
	State m_nextStateId;
};
