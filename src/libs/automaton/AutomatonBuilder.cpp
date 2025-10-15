#include "AutomatonBuilder.h"
#include <fstream>

namespace
{
const std::regex TITLE_REGEX(R"REGEX(\s*digraph\s+(\w+)\s*\{?\s*)REGEX");
const std::regex START_REGEX(R"REGEX(\s*start\s*=\s*(\w+)\s*;?\s*)REGEX");
const std::regex FINAL_REGEX(R"REGEX(\s*final\s*=\s*([^;]+);?\s*)REGEX");
const std::regex TRANSITION_EPS_REGEX(R"REGEX(\s*(\w+)\s*->\s*(\w+)\s*;?\s*)REGEX");
const std::regex TRANSITION_LABEL_REGEX(R"REGEX(\s*(\w+)\s*->\s*(\w+)\s*\[label\s*=\s*"([^"]*)"\]\s*;?\s*)REGEX");

void AssertIsFileOpen(const std::ifstream& file)
{
	if (!file.is_open())
	{
		throw std::invalid_argument("The file cannot be opened");
	}
}

void AssertIsSymbolValid(std::string_view label)
{
	if (label.length() > 1)
	{
		throw std::invalid_argument("There must be one character");
	}
}

std::string_view Trim(std::string_view sv)
{
	const auto first = sv.find_first_not_of(" \t\n\r");
	if (std::string_view::npos == first)
	{
		return {};
	}

	const auto last = sv.find_last_not_of(" \t\n\r");
	return sv.substr(first, last - first + 1);
}

std::set<State> ParseStateList(const std::string& list)
{
	std::stringstream ss(list);
	std::string state;
	std::set<State> states;

	while (std::getline(ss, state, ','))
	{
		auto trimState = Trim(state);
		if (trimState.empty())
		{
			continue;
		}

		states.insert(std::stoul(std::string(trimState)));
	}

	return states;
}
} // namespace

Automaton AutomatonBuilder::FromFile(const std::string& filename)
{
	Automaton automaton;

	std::ifstream file(filename);
	AssertIsFileOpen(file);

	std::string line;
	while (std::getline(file, line))
	{
		ParseLine(automaton, line);
	}

	return automaton;
}

void AutomatonBuilder::ParseLine(Automaton& automaton, const std::string& line)
{
	std::smatch match;

	if (std::regex_search(line, match, TITLE_REGEX))
	{
		HandleTitleDeclaration(automaton, match);
	}
	else if (std::regex_match(line, match, START_REGEX))
	{
		HandleStartDeclaration(automaton, match);
	}
	else if (std::regex_match(line, match, FINAL_REGEX))
	{
		HandleFinalDeclaration(automaton, match);
	}
	else if (std::regex_match(line, match, TRANSITION_LABEL_REGEX))
	{
		HandleLabelTransition(automaton, match);
	}
	else if (std::regex_match(line, match, TRANSITION_EPS_REGEX))
	{
		HandleEpsilonTransition(automaton, match);
	}
}

void AutomatonBuilder::HandleTitleDeclaration(Automaton& automaton,const std::smatch& match)
{
	automaton.SetTitle(match[1].str());
}

void AutomatonBuilder::HandleStartDeclaration(Automaton& automaton,const std::smatch& match)
{
	automaton.SetStartState(std::stoul(match[1].str()));
}

void AutomatonBuilder::HandleFinalDeclaration(Automaton& automaton,const std::smatch& match)
{
	for (const auto& state : ParseStateList(match[1].str()))
	{
		automaton.AddFinalState(state);
	}
}

void AutomatonBuilder::HandleEpsilonTransition(Automaton& automaton,const std::smatch& match)
{
	const State from = std::stoul(match[1].str());
	const State to = std::stoul(match[2].str());

	automaton.AddTransition(from, EPSILON, to);
}

void AutomatonBuilder::HandleLabelTransition(Automaton& automaton,const std::smatch& match)
{
	const State from = std::stoul(match[1].str());
	const State to = std::stoul(match[2].str());
	const std::string labelsStr = match[3].str();

	if (labelsStr.empty())
	{
		automaton.AddTransition(from, EPSILON, to);
	}
	else
	{
		ParseTransitionLabels(automaton, from, to, labelsStr);
	}
}

void AutomatonBuilder::ParseTransitionLabels(Automaton& automaton, State from, State to, const std::string& labels)
{
	std::stringstream ss(labels);
	std::string label;

	while (std::getline(ss, label, ','))
	{
		auto trimLabel = Trim(label);
		if (trimLabel.empty())
		{
			continue;
		}

		AssertIsSymbolValid(trimLabel);
		Symbol symbol = trimLabel[0];
		automaton.AddTransition(from, symbol, to);
	}
}
