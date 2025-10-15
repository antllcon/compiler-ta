#pragma once

#include "Automaton.h"
#include <string>
#include <regex>

class AutomatonBuilder
{
public:
	static Automaton FromFile(const std::string& filename);

private:
	static void ParseLine(Automaton& automaton, const std::string& line);
	static void HandleTitleDeclaration(Automaton& automaton, const std::smatch& match);
	static void HandleStartDeclaration(Automaton& automaton, const std::smatch& match);
	static void HandleFinalDeclaration(Automaton& automaton, const std::smatch& match);
	static void HandleLabelTransition(Automaton& automaton, const std::smatch& match);
	static void HandleEpsilonTransition(Automaton& automaton, const std::smatch& match);
	static void ParseTransitionLabels(Automaton& automaton, State from, State to, const std::string& labels);
};