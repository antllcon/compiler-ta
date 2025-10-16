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

	// void Minimize();
	// void Determine();
	// bool Recognize(const std::string& inputString) const;
	bool IsDeterministic() const;
	void Swap(Automaton& automaton);
	void Clear();

	void SetTitle(const std::string& title);
	void SetStartState(State startState);
	void AddFinalState(State finalState);
	void AddTransition(State from, Symbol on, State to);

	const std::string& GetTitle() const;
	const std::set<State>& GetStates() const;
	const std::set<Symbol>& GetAlphabet() const;
	const std::map<State, std::map<Symbol, std::set<State>>>& GetTransitions() const;
	const std::set<State>& GetFinalStates() const;
	State GetStartState() const;

private:
	std::string m_title;
	std::set<State> m_states;
	std::set<Symbol> m_alphabet;
	std::map<State, std::map<Symbol, std::set<State>>> m_transitions;
	State m_startState = 0;
	std::set<State> m_finalStates;
};