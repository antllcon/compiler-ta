#include "RegexConverterAdapter.h"

#include "RegexRewriter.h"

#include <queue>
#include <set>

namespace
{
constexpr int C_NFA_MATCH = 256;
constexpr int C_NFA_SPLIT = 257;
} // namespace

bool RegexConverterAdapter::Convert(const std::string& regexString, Automaton& automaton) noexcept
{
	automaton.Clear();
	automaton.SetTitle("Regular");

	// 0. Препроцессинг: Разворачиваем {n, m}
	std::string expandedRegex = RegexRewriter::Preprocess(regexString);

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