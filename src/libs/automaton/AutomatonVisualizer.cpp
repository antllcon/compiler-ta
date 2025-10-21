#include "AutomatonVisualizer.h"

#include <fstream>
#include <iomanip>
#include <iostream>

#include <string>
#include <vector>

namespace
{
constexpr int ALPHABET_COL_WIDTH = 15;
constexpr int STATE_COL_WIDTH = 4;

void AssertIsFileOpen(const std::ofstream& file)
{
	if (!file.is_open())
	{
		throw std::invalid_argument("The file cannot be opened");
	}
}
} // namespace

void AutomatonVisualizer::PrintRecognize(const std::string& word, bool result, const std::string& reason)
{
	if (!reason.empty())
	{
		std::cout << "\n";
	}

	std::cout << (result ? "[true] " : "[false]") << "\t" << word << "\n";

	if (!reason.empty())
	{
		std::cout << "Reason: " << reason << "\n\n";
	}
}

void AutomatonVisualizer::TestStrings(const Automaton& automaton, const std::vector<std::string>& words, bool logSteps)
{
	std::cout << "\nRecognition words\n";
	std::cout << "-----------------\n";
	for (const std::string& word : words)
	{
		automaton.Recognize(word, logSteps);
	}
	std::cout << "-----------------\n";
}

void AutomatonVisualizer::Display(const Automaton& automaton)
{
	std::cout << "\nAutomaton";
	if (!automaton.GetTitle().empty())
	{
		std::cout << " " << automaton.GetTitle();
	}
	std::cout << std::endl;

	std::cout << "States (Q):       " << "{ ";
	for (const State state : automaton.GetStates())
	{
		std::cout << state << " ";
	}
	std::cout << "}" << std::endl;

	std::cout << "Alphabet (Σ):     " << "{ ";
	for (const Symbol symbol : automaton.GetAlphabet())
	{
		std::cout << symbol << " ";
	}
	std::cout << "}" << std::endl;

	std::cout << "Start State (q₀): " << "{ " << automaton.GetStartState() << " }" << std::endl;

	std::cout << "Final States (F): " << "{ ";
	for (const State state : automaton.GetFinalStates())
	{
		std::cout << state << " ";
	}
	std::cout << "}" << std::endl;

	std::cout << "Transitions (δ):  " << "{" << std::endl;
	if (automaton.GetTransitions().empty())
	{
		std::cout << "(No transitions defined)" << std::endl;
	}
	else
	{
		for (const auto& fromPair : automaton.GetTransitions())
		{
			for (const auto& onPair : fromPair.second)
			{
				std::cout << "	δ(" << fromPair.first << ", ";
				if (onPair.first == EPSILON)
				{
					std::cout << "ε";
				}
				else
				{
					std::cout << onPair.first;
				}
				std::cout << ") -> { ";
				for (const State toState : onPair.second)
				{
					std::cout << toState << " ";
				}
				std::cout << "}" << std::endl;
			}
		}
	}
	std::cout << "}" << std::endl;
}

void AutomatonVisualizer::ExportToDot(const Automaton& automaton, const std::string& filename)
{
	std::ofstream file(filename);
	AssertIsFileOpen(file);

	file << "digraph " << (automaton.GetTitle().empty() ? filename : automaton.GetTitle()) << std::endl;
	file << " {" << std::endl;

	file << "    start = " << automaton.GetStartState() << ";" << std::endl;
	file << "    final = ";
	std::string finalStatesStr;
	for (const State state : automaton.GetFinalStates())
	{
		finalStatesStr += std::to_string(state) + ", ";
	}
	if (!finalStatesStr.empty())
	{
		finalStatesStr.resize(finalStatesStr.length() - 2);
	}
	file << finalStatesStr << ";" << std::endl
		 << std::endl;

	std::map<std::pair<State, State>, std::vector<Symbol>> transitionsByPair;
	for (const auto& fromPair : automaton.GetTransitions())
	{
		for (const auto& onPair : fromPair.second)
		{
			for (const State toState : onPair.second)
			{
				transitionsByPair[{fromPair.first, toState}].emplace_back(onPair.first);
			}
		}
	}

	for (const auto& pair : transitionsByPair)
	{
		const State from = pair.first.first;
		const State to = pair.first.second;
		const auto& symbols = pair.second;

		std::string labelStr;
		bool hasEpsilonTransition = false;

		for (const Symbol symbol : symbols)
		{
			if (symbol == EPSILON)
			{
				hasEpsilonTransition = true;
			}
			else
			{
				labelStr += std::string(1, symbol) + ", ";
			}
		}

		if (hasEpsilonTransition && !labelStr.empty())
		{
			std::string warningLabels = labelStr;
			warningLabels.resize(warningLabels.length() - 2);
			std::cerr << "[Info - " << filename << "]" << std::endl;
			std::cerr << "Transition " << from << " -> " << to << std::endl;
			std::cerr << "has both ε and symbols {" << warningLabels << "}" << std::endl;
		}

		if (!labelStr.empty())
		{
			labelStr.resize(labelStr.length() - 2);
			file << "    " << from << " -> " << to << " [label = \"" << labelStr << "\"];" << std::endl;
		}

		if (hasEpsilonTransition)
		{
			file << "    " << from << " -> " << to << ";" << std::endl;
		}
	}

	file << "}" << std::endl;
}

void AutomatonVisualizer::PrintMinimizationTable(
	const std::vector<Symbol>& alphabet,
	const std::vector<std::set<State>>& partitions,
	const std::map<State, std::vector<int>>& stateSignatures,
	int iterationNumber)
{
	auto getClassName = [iterationNumber](int classIndex) {
		char letter = 'A' + (iterationNumber - 1);
		return letter + std::to_string(classIndex + 1);
	};

	// Рассчитываем общую ширину таблицы
	size_t totalWidth = ALPHABET_COL_WIDTH + 1;
	for (const auto& partition : partitions)
	{
		totalWidth += partition.size() * (STATE_COL_WIDTH + 1);
	}

	auto printLine = [&]() {
		std::cout << std::string(totalWidth, '-') << "\n";
	};

	printLine();

	// Печать заголовка с классами
	std::cout << "| " << std::left << std::setw(ALPHABET_COL_WIDTH - 2) << "Class" << "|";
	for (int i = 0; i < partitions.size(); ++i)
	{
		size_t partitionWidth = partitions[i].size() * STATE_COL_WIDTH - 1 + (partitions[i].size() - 1);
		std::cout << " " << std::left << std::setw(partitionWidth) << getClassName(i) << "|";
	}
	std::cout << "\n";
	printLine();

	// Печать заголовка с состояниями
	std::cout << "| " << std::left << std::setw(ALPHABET_COL_WIDTH - 2) << "Alph / State" << "|";
	for (const auto& partition : partitions)
	{
		for (const State state : partition)
		{
			std::cout << " " << std::left << std::setw(STATE_COL_WIDTH - 1) << state << "|";
		}
	}
	std::cout << "\n";
	printLine();

	// Печать строк с данными
	for (int i = 0; i < alphabet.size(); ++i)
	{
		std::cout << "| " << std::left << std::setw(ALPHABET_COL_WIDTH - 2) << alphabet[i] << "|";
		for (const auto& partition : partitions)
		{
			for (const State state : partition)
			{
				const auto& signature = stateSignatures.at(state);
				int destClassId = signature[i];

				std::string destClassName = (destClassId == -1) ? "-" : getClassName(destClassId);

				std::cout << " " << std::left << std::setw(STATE_COL_WIDTH - 1) << destClassName << "|";
			}
		}
		std::cout << "\n";
	}
	printLine();
}

void AutomatonVisualizer::PrintDeterminizationTable(
	const std::vector<Symbol>& alphabet,
	const DfaTransitionTable& dfaTransitions)
{
	if (dfaTransitions.empty())
	{
		return;
	}

	// Вспомогательная лямбда для красивого вывода множества состояний
	auto statesToString = [](const DfaStateKey& states) -> std::string {
		if (states.empty()) return "-";
		std::stringstream ss;
		ss << "{";
		for (auto it = states.begin(); it != states.end(); ++it)
		{
			ss << *it << (std::next(it) == states.end() ? "" : ",");
		}
		ss << "}";
		return ss.str();
	};

	// Динамический расчет ширины колонок
	size_t firstColWidth = 12;
	std::map<Symbol, size_t> columnWidths;

	for (Symbol s : alphabet)
	{
		columnWidths[s] = std::string(1, s).length();
	}

	for (const auto& row : dfaTransitions)
	{
		firstColWidth = std::max(firstColWidth, statesToString(row.first).length());
		for (Symbol s : alphabet)
		{
			if (row.second.contains(s))
			{
				columnWidths[s] = std::max(columnWidths[s], statesToString(row.second.at(s)).length());
			}
		}
	}

	firstColWidth += 4;
	for (Symbol s : alphabet)
	{
		columnWidths[s] += 4;
	}

	auto printLine = [&]() {
		std::cout << std::string(firstColWidth, '-');
		for (Symbol s : alphabet)
		{
			std::cout << std::string(columnWidths.at(s), '-');
		}
		std::cout << "-\n";
	};

	// Печать таблицы
	printLine();

	// Заголовок
	std::cout << "|" << std::left << std::setw(firstColWidth - 1) << " State / Alph" << "|";
	for (Symbol s : alphabet)
	{
		std::cout << std::left << std::setw(columnWidths.at(s) - 1) << (" " + std::string(1, s)) << "|";
	}
	std::cout << "\n";
	printLine();

	// Строки с данными
	for (const auto& row : dfaTransitions)
	{
		std::cout << "|" << std::left << std::setw(firstColWidth - 1) << (" " + statesToString(row.first)) << "|";
		for (Symbol s : alphabet)
		{
			std::string cellContent = "-";
			if (row.second.contains(s))
			{
				cellContent = statesToString(row.second.at(s));
			}
			std::cout << std::left << std::setw(columnWidths.at(s) - 1) << (" " + cellContent) << "|";
		}
		std::cout << "\n";
	}
	printLine();
}