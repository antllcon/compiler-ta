#include "RegexConverterAdapter.h"
#include <queue>
#include <set>

namespace
{
constexpr int C_NFA_MATCH = 256;
constexpr int C_NFA_SPLIT = 257;

std::string PreprocessRangeQuantifiers(const std::string& input)
{
	std::string res;
	res.reserve(input.size() * 2);

	for (size_t i = 0; i < input.size(); ++i)
	{
		if (input[i] == '{')
		{
			size_t closePos = input.find('}', i);
			if (closePos == std::string::npos)
			{
				res += input[i];
				continue;
			}

			std::string content = input.substr(i + 1, closePos - i - 1);
			int minRep = 0;
			int maxRep = -1;

			size_t commaPos = content.find(',');
			try
			{
				if (commaPos == std::string::npos)
				{
					minRep = std::stoi(content);
					maxRep = minRep;
				}
				else
				{
					std::string nStr = content.substr(0, commaPos);
					minRep = std::stoi(nStr);

					if (commaPos + 1 < content.size())
					{
						maxRep = std::stoi(content.substr(commaPos + 1));
					}
					else
					{
						maxRep = -1;
					}
				}
			}
			catch (...)
			{
				res += input[i];
				continue;
			}

			if (res.empty()) return "";

			std::string lastAtom;
			if (res.back() == ')')
			{
				int balance = 1;
				size_t j = res.size() - 2;
				bool found = false;
				while (j < res.size())
				{
					if (res[j] == ')') balance++;
					if (res[j] == '(') balance--;
					if (balance == 0)
					{
						lastAtom = res.substr(j);
						res.resize(j);
						found = true;
						break;
					}
					j--;
				}
				if (!found) return "";
			}
			else
			{
				lastAtom = res.back();
				res.pop_back();
			}

			for (int k = 0; k < minRep; ++k)
			{
				res += lastAtom;
			}

			if (maxRep == -1)
			{
				res += lastAtom;
				res += '*';
			}
			else
			{
				for (int k = 0; k < maxRep - minRep; ++k)
				{
					res += lastAtom;
					res += '?';
				}
			}

			i = closePos;
		}
		else if (input[i] == '\\')
		{
			res += input[i];
			if (i + 1 < input.size())
			{
				res += input[i + 1];
				i++;
			}
		}
		else
		{
			res += input[i];
		}
	}
	return res;
}
} // namespace

bool RegexConverterAdapter::Convert(const std::string& regexString, Automaton& automaton) noexcept
{
	automaton.Clear();
	automaton.SetTitle("Regular");

	// 0. Препроцессинг: Разворачиваем {n, m}
	std::string expandedRegex = PreprocessRangeQuantifiers(regexString);

	// 1. Вызов C-API (Adaptee)
	std::vector buffer(expandedRegex.begin(), expandedRegex.end());
	buffer.push_back('\0');

	char* post = re2post(buffer.data());
	if (post == nullptr)
	{
		return false;
	}

	CState* cStartState = post2nfa(post);
	if (cStartState == nullptr)
	{
		return false;
	}

	// 2. Конвертация (Логика Адаптера)
	ConversionContext context{automaton};
	std::queue<CState*> workQueue;
	std::set<CState*> visited;

	// 2.1. Инициализируем стартовое состояние
	State cppStartState = GetOrCreateState(context, cStartState);
	automaton.SetStartState(cppStartState);

	workQueue.push(cStartState);
	visited.insert(cStartState);

	// 2.2. Запускаем обход в ширину (BFS) по C-графу
	while (!workQueue.empty())
	{
		CState* cCurrent = workQueue.front();
		workQueue.pop();

		State cppFromState = context.cStateToCppState.at(cCurrent);

		switch (cCurrent->c)
		{
		case C_NFA_MATCH:
			break;

		case C_NFA_SPLIT:
			if (cCurrent->out)
			{
				State cppTo1 = GetOrCreateState(context, cCurrent->out);
				automaton.AddTransition(cppFromState, EPSILON, cppTo1);

				auto [_, inserted] = visited.insert(cCurrent->out);
				if (inserted)
				{
					workQueue.push(cCurrent->out);
				}
			}
			if (cCurrent->out1)
			{
				State cppTo2 = GetOrCreateState(context, cCurrent->out1);
				automaton.AddTransition(cppFromState, EPSILON, cppTo2);

				auto [_, inserted] = visited.insert(cCurrent->out1);
				if (inserted)
				{
					workQueue.push(cCurrent->out1);
				}
			}
			break;

		default:
			if (cCurrent->out)
			{
				auto symbol = static_cast<Symbol>(cCurrent->c);
				State cppTo = GetOrCreateState(context, cCurrent->out);
				automaton.AddTransition(cppFromState, symbol, cppTo);

				auto [_, inserted] = visited.insert(cCurrent->out);

				if (inserted)
				{
					workQueue.push(cCurrent->out);
				}
			}
			break;
		}
	}

	return true;
}

State RegexConverterAdapter::GetOrCreateState(ConversionContext& context, CState* cState)
{
	auto [iterator, inserted] = context.cStateToCppState.try_emplace(cState, 0);

	if (inserted)
	{
		State newStateId = context.nextStateId++;
		iterator->second = newStateId;

		if (cState == &matchstate)
		{
			context.cppNFA.AddFinalState(newStateId);
		}
	}

	return iterator->second;
}