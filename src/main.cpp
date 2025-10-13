#include "Automaton.h"

#include <cstdlib>
#include <iostream>

int main()
{
	std::cout << "Program compiler-ta" << std::endl;
	const Automaton automaton("example.dot");
	automaton.Display();
	return EXIT_SUCCESS;
}