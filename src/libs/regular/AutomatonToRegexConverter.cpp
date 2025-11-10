#include "AutomatonToRegexConverter.h"

#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

namespace
{
using Regex = std::string;

const Regex EMPTY_SET = "{}";
const Regex EPSILON_STR(1, EPSILON);

// Объединение (Union) двух РВ (R1 | R2)
Regex Union(const Regex& r1, const Regex& r2)
{
	// R | {} = R
	if (r1 == EMPTY_SET)
	{
		return r2;
	}
	// {} | R = R
	if (r2 == EMPTY_SET)
	{
		return r1;
	}
	// R | R = R
	if (r1 == r2)
	{
		return r1;
	}
	return "(" + r1 + "|" + r2 + ")";
}

// Конкатенация двух РВ (R1 R2)
Regex Concat(const Regex& r1, const Regex& r2)
{
	// R . {} = {}
	if (r1 == EMPTY_SET)
	{
		return EMPTY_SET;
	}
	// {} . R = {}
	if (r2 == EMPTY_SET)
	{
		return EMPTY_SET;
	}
	// R . e = R
	if (r1 == EPSILON_STR)
	{
		return r2;
	}
	// e . R = R
	if (r2 == EPSILON_STR)
	{
		return r1;
	}

	// (a|b)c = (a|b)c, a(b|c) = a(b|c)
	std::string f1 = r1;
	if (r1.find('|') != std::string::npos)
	{
		f1 = "(" + r1 + ")";
	}

	std::string f2 = r2;
	if (r2.find('|') != std::string::npos)
	{
		f2 = "(" + r2 + ")";
	}

	return f1 + f2;
}

// Замыкание Клини (R*)
Regex Kleene(const Regex& r)
{
	// {}* = e
	if (r == EMPTY_SET)
	{
		return EPSILON_STR;
	}
	// e* = e
	if (r == EPSILON_STR)
	{
		return EPSILON_STR;
	}
	// (R*)* = R*
	if (r.length() > 1 && r.back() == '*')
	{
		if (r[0] == '(' && r[r.length() - 2] == ')')
		{
			return r;
		}
	}
	// (a)* -> a*
	if (r.length() == 1)
	{
		return r + "*";
	}
	// (ab)*, (a|b)*
	return "(" + r + ")*";
}

class ConversionProcess
{
public:
	explicit ConversionProcess(const Automaton& automaton)
		: m_nextStateId(0)
		, m_newStart(0)
		, m_newFinal(0)
	{
		Prepare(automaton);
	}

	Regex Run()
	{
		EliminateStates();
		return GetFinalRegex();
	}

private:
	void Prepare(const Automaton& automaton)
	{
		const auto& originalStates = automaton.GetStates();
		if (originalStates.empty())
		{
			return;
		}

		State maxId = *std::ranges::max_element(originalStates);
		m_nextStateId = maxId + 1;
		m_newStart = m_nextStateId++;
		m_newFinal = m_nextStateId++;

		m_intermediateStates = originalStates;

		std::set<State> allStates = originalStates;
		allStates.insert(m_newStart);
		allStates.insert(m_newFinal);

		for (State s1 : allStates)
		{
			for (State s2 : allStates)
			{
				m_gnfa[s1][s2] = (s1 == s2) ? EPSILON_STR : EMPTY_SET;
			}
		}

		for (const auto& [from, transitions] : automaton.GetTransitions())
		{
			for (const auto& [symbol, toStates] : transitions)
			{
				Regex label = (symbol == EPSILON)
					? EPSILON_STR
					: std::string(1, symbol);

				for (State to : toStates)
				{
					m_gnfa[from][to] = Union(m_gnfa[from][to], label);
				}
			}
		}

		m_gnfa[m_newStart][automaton.GetStartState()] = EPSILON_STR;
		for (State finalState : automaton.GetFinalStates())
		{
			m_gnfa[finalState][m_newFinal] = EPSILON_STR;
		}
	}

	void EliminateStates()
	{
		std::set<State> allNodes = m_intermediateStates;
		allNodes.insert(m_newStart);
		allNodes.insert(m_newFinal);

		for (State q_elim : m_intermediateStates)
		{
			std::vector<State> nodesToUpdate;
			for (State s : allNodes)
			{
				if (s != q_elim)
				{
					nodesToUpdate.push_back(s);
				}
			}

			for (State qi : nodesToUpdate)
			{
				for (State qj : nodesToUpdate)
				{
					Regex R_ik = m_gnfa[qi][q_elim];
					Regex R_kk = m_gnfa[q_elim][q_elim];
					Regex R_kj = m_gnfa[q_elim][qj];
					Regex Old_R_ij = m_gnfa[qi][qj];

					if (R_ik == EMPTY_SET || R_kj == EMPTY_SET)
					{
						continue;
					}

					Regex newPath = Concat(Concat(R_ik, Kleene(R_kk)), R_kj);

					m_gnfa[qi][qj] = Union(Old_R_ij, newPath);
				}
			}
		}
	}

	Regex GetFinalRegex()
	{
		if (m_intermediateStates.empty())
		{
			return EMPTY_SET;
		}

		Regex finalRegex = m_gnfa[m_newStart][m_newFinal];

		if (finalRegex.find(EPSILON_STR + "|") == 1)
		{
		}

		return finalRegex;
	}


	std::map<State, std::map<State, Regex>> m_gnfa;
	std::set<State> m_intermediateStates;
	State m_nextStateId;
	State m_newStart;
	State m_newFinal;
};

} // namespace

std::string AutomatonToRegexConverter::Convert(const Automaton& automaton)
{
	ConversionProcess converter(automaton);
	return converter.Run();
}