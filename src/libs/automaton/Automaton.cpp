#include "Automaton.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace
{
constexpr Symbol EPSILON = '\0';
const std::regex START_REGEX(R"REGEX(\s*start\s*=\s*(\w+)\s*;?)REGEX");
const std::regex FINAL_REGEX(R"REGEX(\s*final\s*=\s*([^;]+);?)REGEX");
const std::regex TRANSITION_EPS_REGEX(R"REGEX(\s*(\w+)\s*->\s*(\w+)\s*;?)REGEX");
const std::regex TRANSITION_LABEL_REGEX(R"REGEX(\s*(\w+)\s*->\s*(\w+)\s*\[label\s*=\s*"([^"]*)"\]\s*;?)REGEX");

void AssertIsFileOpen(const std::ifstream& file)
{
	if (!file.is_open())
	{
		throw std::runtime_error("Не могу открыть файл");
	}
}

void AssertIsSymbolValid(std::string_view label)
{
	if (label.length() > 1)
	{
		throw std::invalid_argument("Должен быть 1 символ");
	}
}

std::string_view Trim(std::string_view sv)
{
	const auto first = sv.find_first_not_of('\t');
	const auto last = sv.find_last_not_of('\t');
	return sv.substr(first, last - first + 1);
}

void ParseStateList(const std::string& list, std::set<State>& states)
{
	std::stringstream ss(list);
	std::string state;

	while (std::getline(ss, state, ','))
	{
		auto trimState = Trim(state);
		if (trimState.empty())
		{
			continue;
		}

		AssertIsSymbolValid(trimState);
		if (!trimState.empty())
		{
			states.insert(std::stoul(std::string(trimState)));
		}
	}
}
} // namespace

Automaton::Automaton(const std::string& filename)
{
	std::ifstream file(filename);
	AssertIsFileOpen(file);

	std::string line;
	while (std::getline(file, line))
	{
		std::smatch match;
		ParseLine(line);
	}
}

void Automaton::ParseLine(const std::string& line)
{
	std::smatch match;

	if (std::regex_match(line, match, START_REGEX))
	{
		HandleStartDeclaration(match);
	}
	else if (std::regex_match(line, match, FINAL_REGEX))
	{
		HandleFinalDeclaration(match);
	}
	else if (std::regex_match(line, match, TRANSITION_EPS_REGEX))
	{
		HandleEpsilonTransition(match);
	}
	else if (std::regex_match(line, match, TRANSITION_LABEL_REGEX))
	{
		HandleLabelTransition(match);
	}
}

void Automaton::HandleStartDeclaration(const std::smatch& match)
{
	m_startState = std::stoul(match[1].str());
	m_states.insert(m_startState);
}

void Automaton::HandleFinalDeclaration(const std::smatch& match)
{
	ParseStateList(match[1].str(), m_finalStates);
	for (const auto& state : m_finalStates)
	{
		m_states.insert(state);
	}
}

void Automaton::HandleEpsilonTransition(const std::smatch& match)
{
	const State from = std::stoul(match[1].str());
	const State to = std::stoul(match[2].str());

	m_states.insert(from);
	m_states.insert(to);
	m_transitions[from][EPSILON].insert(to);
}

void Automaton::HandleLabelTransition(const std::smatch& match)
{
	const State from = std::stoul(match[1].str());
	const State to = std::stoul(match[2].str());
	const std::string labelsStr = match[3].str();

	m_states.insert(from);
	m_states.insert(to);

	if (labelsStr.empty())
	{
		m_transitions[from][EPSILON].insert(to);
	}
	else
	{
		ParseTransitionLabels(from, to, labelsStr);
	}
}

void Automaton::ParseTransitionLabels(State from, State to, const std::string& labels)
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
		m_alphabet.insert(symbol);
		m_transitions[from][symbol].insert(to);
	}
}

void Automaton::Display() const
{
	std::cout << "--- Automaton Definition ---\n";

	// 1. Вывод всех состояний (Q)
	std::cout << "States (Q):      { ";
	for (const State state : m_states)
	{
		std::cout << state << " ";
	}
	std::cout << "}\n";

	// 2. Вывод алфавита (Σ)
	std::cout << "Alphabet (Σ):    { ";
	for (const Symbol symbol : m_alphabet)
	{
		std::cout << symbol << " ";
	}
	std::cout << "}\n";

	// 3. Вывод начального состояния (q₀)
	std::cout << "Start State (q₀): " << m_startState << "\n";

	// 4. Вывод конечных состояний (F)
	std::cout << "Final States (F):  { ";
	for (const State state : m_finalStates)
	{
		std::cout << state << " ";
	}
	std::cout << "}\n";

	// 5. Вывод функции переходов (δ)
	std::cout << "Transitions (δ):\n";
	if (m_transitions.empty())
	{
		std::cout << "  (No transitions defined)\n";
	}
	else
	{
		for (const auto& fromPair : m_transitions)
		{
			const State fromState = fromPair.first;
			for (const auto& onPair : fromPair.second)
			{
				const Symbol onSymbol = onPair.first;
				const std::set<State>& toStates = onPair.second;

				// Для каждого перехода выводим отдельную строку
				std::cout << "  δ(" << fromState << ", ";

				// Красиво выводим эпсилон-символ
				if (onSymbol == EPSILON)
				{
					std::cout << "ε";
				}
				else
				{
					std::cout << onSymbol;
				}

				std::cout << ") -> { ";
				for (const State toState : toStates)
				{
					std::cout << toState << " ";
				}
				std::cout << "}\n";
			}
		}
	}

	std::cout << "--------------------------\n";
}