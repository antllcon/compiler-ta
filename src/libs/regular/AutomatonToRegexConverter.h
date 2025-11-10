#pragma once

#include "../automaton/Automaton.h"
#include <string>

class AutomatonToRegexConverter
{
public:
	AutomatonToRegexConverter() = delete;
	~AutomatonToRegexConverter() = delete;

	static std::string Convert(const Automaton& automaton);
};