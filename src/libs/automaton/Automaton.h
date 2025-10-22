#pragma once

#include <map>
#include <set>
#include <string>
#include <regex>

using Symbol = unsigned char;
using State = unsigned int;
constexpr Symbol EPSILON = 'e';

class Automaton
{
public:
	Automaton() = default;
	~Automaton() = default;

	void SetTitle(const std::string& title);
	const std::string& GetTitle() const;

	void SetStartState(State startState);
	State GetStartState() const;

	void AddFinalState(State finalState);
	const std::set<State>& GetFinalStates() const;

	void AddTransition(State from, Symbol on, State to);
	const std::map<State, std::map<Symbol, std::set<State>>>& GetTransitions() const;

	const std::set<State>& GetStates() const;
	const std::set<Symbol>& GetAlphabet() const;

	bool Recognize(const std::string& inputString, bool logSteps = false) const;
	void Swap(Automaton& automaton);
	void Clear();

	bool IsDeterministic() const;

private:
	std::string m_title;
	std::set<State> m_states;
	std::set<Symbol> m_alphabet;
	std::map<State, std::map<Symbol, std::set<State>>> m_transitions;
	State m_startState = 0;
	std::set<State> m_finalStates;
};