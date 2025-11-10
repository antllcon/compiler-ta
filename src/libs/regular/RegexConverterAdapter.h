#pragma once

#include "../automaton/Automaton.h"
#include "nfa_c_api.h"
#include <string>
#include <map>

class RegexConverterAdapter
{
	struct ConversionContext
	{
		Automaton& cppNFA;
		std::map<CState*, State> cStateToCppState;
		State nextStateId = 0;
	};

public:
	RegexConverterAdapter() = default;
	~RegexConverterAdapter() = default;

	RegexConverterAdapter(const RegexConverterAdapter&) = delete;
	RegexConverterAdapter& operator=(const RegexConverterAdapter&) = delete;

	static std::string Convert(const Automaton& automaton);
	static bool Convert(const std::string& regexString, Automaton& automaton) noexcept;

private:
	static State GetOrCreateState(ConversionContext& context, CState* cState);
};