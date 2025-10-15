#pragma once

#include "Automaton.h"
#include <string>

class AutomatonVisualizer
{
public:
	// TODO: добавить метод перевода в строку
    static void Display(const Automaton& automaton);
    static void ExportToDot(const Automaton& automaton, const std::string& filename);
};