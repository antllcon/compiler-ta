#pragma once

#include <map>
#include <set>
#include <string>
#include <regex>

using Symbol = unsigned char;
using State = unsigned int;

class Automaton
{
public:
	explicit Automaton(const std::string& filename);
	~Automaton() = default;

	// void Minimize();
	// void Determine();
	//
	// bool Recognize(const std::string& inputString) const;
	void Display() const;

private:
	void ParseLine(const std::string& line);
	void HandleStartDeclaration(const std::smatch& match);
	void HandleFinalDeclaration(const std::smatch& match);
	void HandleLabelTransition(const std::smatch& match);
	void HandleEpsilonTransition(const std::smatch& match);
	void ParseTransitionLabels(State from, State to, const std::string& labels);

	std::set<State> m_states;
	std::set<Symbol> m_alphabet;
	std::map<State, std::map<Symbol, std::set<State>>> m_transitions;
	State m_startState = 0;
	std::set<State> m_finalStates;
};