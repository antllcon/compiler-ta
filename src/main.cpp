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
		auto automaton = AutomatonBuilder::FromFile("input/home.dot");
		auto determination = DeterminizationAlgorithm::Determine(automaton, true);
		// auto minimization = MinimizationAlgorithm::Minimize(determination, true);

		const std::vector<std::string> testWords = {
			"1101",
			"1111101",
			"0",
			"0001",
			"11010",
			"1", // Ошибочный (нет подходящего конечного состояния)
			"1101",
			"11101",
			"000",
			"000001",
			"11010"};

		AutomatonVisualizer::TestStrings(determination, testWords, true);
		AutomatonVisualizer::ExportToDot(determination, "output/test.dot");
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