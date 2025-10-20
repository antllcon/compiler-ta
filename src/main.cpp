#define EXIT_DATA 2
#include "Automaton.h"
#include "AutomatonBuilder.h"
#include "AutomatonVisualizer.h"
#include "MinimizationAlghoritm.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>

int main()
{
	// Должна быть директория (res), можно указать в параметрах сборки
	std::cout << "Current directory: " << std::filesystem::current_path() << std::endl;

	try
	{
		auto automaton = AutomatonBuilder::FromFile("input/itmo.dot");
		auto minAutomaton = MinimizationAlgorithm::Minimize(automaton, true);

		AutomatonVisualizer::Display(minAutomaton);
		AutomatonVisualizer::ExportToDot(minAutomaton, "output/test.dot");
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