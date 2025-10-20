#include "Automaton.h"
#include "MinimizationAlghoritm.h"
#include <gtest/gtest.h>

class MinimizationTest : public ::testing::Test
{
protected:
	Automaton automaton;

	static size_t GetTransitionCount(const Automaton& dfa)
	{
		size_t count = 0;
		for (const auto& [fromState, transitions] : dfa.GetTransitions())
		{
			for (const auto& [symbol, toStates] : transitions)
			{
				count += toStates.size();
			}
		}
		return count;
	}
};

// Минимизированный автомат не меняется
TEST_F(MinimizationTest, DoesNotChangeAlreadyMinimizedDfa)
{
	automaton.SetStartState(0);
	automaton.AddFinalState(2);
	automaton.AddTransition(0, 'a', 1);
	automaton.AddTransition(1, 'b', 2);

	constexpr size_t expectedStateCount = 3;
	EXPECT_EQ(automaton.GetStates().size(), expectedStateCount);

	auto min = MinimizationAlgorithm::Minimize(automaton);

	EXPECT_EQ(min.GetStates().size(), expectedStateCount);
	EXPECT_EQ(min.GetFinalStates().size(), 1);
	EXPECT_EQ(min.GetTransitions().size(), 2);
}

// Алгоритм выбрасывает исключение для НКА
TEST_F(MinimizationTest, ThrowsExceptionForNondeterministicAutomaton)
{
	automaton.SetStartState(0);
	automaton.AddFinalState(2);
	automaton.AddTransition(0, 'a', 1);
	automaton.AddTransition(0, 'a', 2);

	EXPECT_THROW(auto min = MinimizationAlgorithm::Minimize(automaton), std::logic_error);
}

// Проверяем обработку пустого автомата
TEST_F(MinimizationTest, HandlesEmptyAutomaton)
{
	EXPECT_EQ(automaton.GetStates().size(), 0);
	EXPECT_NO_THROW(MinimizationAlgorithm::Minimize(automaton));
	EXPECT_EQ(automaton.GetStates().size(), 0);
}

// Проверяем автомат с одним состоянием
TEST_F(MinimizationTest, HandlesSingleStateAutomaton)
{
	automaton.SetStartState(0);
	automaton.AddFinalState(0);
	automaton.AddTransition(0, 'a', 0);

	EXPECT_EQ(automaton.GetStates().size(), 1);
	EXPECT_NO_THROW(auto min = MinimizationAlgorithm::Minimize(automaton));

	auto min = MinimizationAlgorithm::Minimize(automaton);
	EXPECT_EQ(min.GetStates().size(), 1);
}

// Слияние двух эквивалентных НЕ-финальных состояний
TEST_F(MinimizationTest, MinimizesEquivalentNonFinalStates)
{
    automaton.SetStartState(0);
    automaton.AddFinalState(3);
    automaton.AddTransition(0, 'a', 1);
    automaton.AddTransition(0, 'b', 2);
    automaton.AddTransition(1, 'x', 3);
    automaton.AddTransition(2, 'x', 3);

    EXPECT_EQ(automaton.GetStates().size(), 4);
    EXPECT_EQ(automaton.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(automaton), 4);

    Automaton minimized = MinimizationAlgorithm::Minimize(automaton);

    EXPECT_EQ(minimized.GetStates().size(), 3);
    EXPECT_EQ(minimized.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(minimized), 3);
}

// Слияние двух эквивалентных ФИНАЛЬНЫХ состояний
TEST_F(MinimizationTest, MinimizesEquivalentFinalStates)
{
    automaton.SetStartState(0);
    automaton.AddFinalState(1);
    automaton.AddFinalState(2);
    automaton.AddTransition(0, 'a', 1);
    automaton.AddTransition(0, 'b', 2);
    automaton.AddTransition(1, 'x', 1);
    automaton.AddTransition(2, 'x', 2);

    EXPECT_EQ(automaton.GetStates().size(), 3);
    EXPECT_EQ(automaton.GetFinalStates().size(), 2);
    EXPECT_EQ(GetTransitionCount(automaton), 4);

    Automaton minimized = MinimizationAlgorithm::Minimize(automaton);

    EXPECT_EQ(minimized.GetStates().size(), 2);
    EXPECT_EQ(minimized.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(minimized), 3);
}

// Удаление недостижимых состояний
TEST_F(MinimizationTest, RemovesUnreachableStates)
{
    automaton.SetStartState(0);
    automaton.AddFinalState(1);
    automaton.AddFinalState(3);
    automaton.AddTransition(0, 'a', 1);
    automaton.AddTransition(2, 'b', 3);

    EXPECT_EQ(automaton.GetStates().size(), 4);
    EXPECT_EQ(automaton.GetFinalStates().size(), 2);
    EXPECT_EQ(GetTransitionCount(automaton), 2);

    Automaton minimized = MinimizationAlgorithm::Minimize(automaton);

    EXPECT_EQ(minimized.GetStates().size(), 2);
    EXPECT_EQ(minimized.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(minimized), 1);
}

// Более сложный случай
TEST_F(MinimizationTest, HandlesMultiStepRefinement)
{
    automaton.SetStartState(0);
    automaton.AddFinalState(2);
    automaton.AddFinalState(3);

    automaton.AddTransition(0, 'a', 1); automaton.AddTransition(0, 'b', 4);
    automaton.AddTransition(1, 'a', 2); automaton.AddTransition(1, 'b', 4);
    automaton.AddTransition(2, 'a', 3); automaton.AddTransition(2, 'b', 4);
    automaton.AddTransition(3, 'a', 3); automaton.AddTransition(3, 'b', 4);
    automaton.AddTransition(4, 'a', 4); automaton.AddTransition(4, 'b', 4);


    EXPECT_EQ(automaton.GetStates().size(), 5);
    EXPECT_EQ(automaton.GetFinalStates().size(), 2);
    EXPECT_EQ(GetTransitionCount(automaton), 10);

    Automaton minimized = MinimizationAlgorithm::Minimize(automaton);

    EXPECT_EQ(minimized.GetStates().size(), 4);
    EXPECT_EQ(minimized.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(minimized), 8);
}