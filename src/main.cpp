#define EXIT_DATA 2
#include "Automaton.h"
#include "AutomatonBuilder.h"
#include "AutomatonVisualizer.h"
#include "DeterminizationAlgorithm.h"
#include "MinimizationAlgorithm.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>

int main()
{
	// Должна быть директория (res), можно указать в параметрах сборки
	std::cout << "Current directory: " << std::filesystem::current_path() << std::endl;

	try
	{

		auto awt = AutomatonBuilder::FromFile("test.dot");
		auto dwt = DeterminizationAlgorithm::Determine(awt, true);
		auto min = MinimizationAlgorithm::Minimize(dwt, true);
		AutomatonVisualizer::ExportToDot(min, "output/result.dot");

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