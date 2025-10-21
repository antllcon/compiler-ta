#define EXIT_DATA 2
#include "Automaton.h"
#include "AutomatonBuilder.h"
#include "AutomatonVisualizer.h"
#include "DeterminizationAlgorithm.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>

int main()
{
	// Должна быть директория (res), можно указать в параметрах сборки
	std::cout << "Current directory: " << std::filesystem::current_path() << std::endl;

	try
	{
		auto automaton = AutomatonBuilder::FromFile("input/home.dot");
		auto determination = DeterminizationAlgorithm::Determine(automaton, true);
		AutomatonVisualizer::ExportToDot(determination, "output/test.dot");


		// auto minimization = MinimizationAlgorithm::Minimize(determination, true);

		// AutomatonVisualizer::Display(minimization);
		// AutomatonVisualizer::ExportToDot(minimization, "output/test.dot");
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