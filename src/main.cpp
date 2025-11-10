#define EXIT_DATA 2
#include "Automaton.h"
#include "AutomatonBuilder.h"
#include "AutomatonToRegexConverter.h"
#include "AutomatonVisualizer.h"
#include "DeterminizationAlgorithm.h"
#include "MinimizationAlgorithm.h"
#include "RegexConverterAdapter.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>

int main()
{
	// Должна быть директория (res), можно указать в параметрах сборки
	std::cout << "Current directory: " << std::filesystem::current_path() << std::endl;

	try
	{
		Automaton nfa;
		RegexConverterAdapter adapter;
		std::string regex("(a|b)*|a*(a|b)(a|b)");

		if (adapter.Convert(regex, nfa))
		{
			auto detNfa = DeterminizationAlgorithm::Determine(nfa);
			auto minNfa = MinimizationAlgorithm::Minimize(detNfa);
			// AutomatonVisualizer::ExportToDot(minNfa, "output/min.dot");
			AutomatonVisualizer::ExportToDot(detNfa, "output/det.dot");

			auto regexFromAutomaton = AutomatonToRegexConverter::Convert(minNfa);
			std::cout << "\nРегулярное выражение из автомата: "<< regexFromAutomaton << std::endl;
		}
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << "Data error: " << e.what() << std::endl;
		return EXIT_DATA;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}