#pragma once

#include "Grammar.h"
#include "../automaton/Automaton.h"

class GrammarToNfaConverter
{
public:
	GrammarToNfaConverter() = delete;
	~GrammarToNfaConverter() = delete;

	static Automaton Convert(const Grammar& grammar);

private:
	static Automaton ConvertRightLinear(const Grammar& grammar);
	static Automaton ConvertLeftLinear(const Grammar& grammar);
};