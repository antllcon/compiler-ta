#include "Automaton.h"
#include "DeterminizationAlgorithm.h"
#include <gtest/gtest.h>

class DeterminizationTest : public ::testing::Test
{
protected:
    Automaton nfa;

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

// Пустой автомат
TEST_F(DeterminizationTest, HandlesEmptyAutomaton)
{
    EXPECT_EQ(nfa.GetStates().size(), 0);

    Automaton dfa = DeterminizationAlgorithm::Determine(nfa);

    EXPECT_EQ(dfa.GetStates().size(), 0);
    EXPECT_EQ(GetTransitionCount(dfa), 0);
}

// Автомат уже является ДКА
TEST_F(DeterminizationTest, HandlesAlreadyDeterministic)
{
    nfa.SetStartState(0);
    nfa.AddFinalState(2);
    nfa.AddTransition(0, 'a', 1);
    nfa.AddTransition(1, 'b', 2);

    Automaton dfa = DeterminizationAlgorithm::Determine(nfa);

    EXPECT_EQ(dfa.GetStates().size(), 3);
    EXPECT_EQ(dfa.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(dfa), 2);
}

// Простая недетерминированность по одному символу
TEST_F(DeterminizationTest, ResolvesSimpleNondeterminism)
{
    nfa.SetStartState(0);
    nfa.AddFinalState(1);
    nfa.AddTransition(0, 'a', 1);
    nfa.AddTransition(0, 'a', 2);

    Automaton dfa = DeterminizationAlgorithm::Determine(nfa);

    EXPECT_EQ(dfa.GetStates().size(), 2);
    EXPECT_EQ(dfa.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(dfa), 1);
}

// e-замыкание из начального состояния
TEST_F(DeterminizationTest, HandlesEpsilonClosureFromStart)
{
    nfa.SetStartState(0);
    nfa.AddFinalState(2);
    nfa.AddTransition(0, EPSILON, 1);
    nfa.AddTransition(1, 'a', 2);

    Automaton dfa = DeterminizationAlgorithm::Determine(nfa);

    EXPECT_EQ(dfa.GetStates().size(), 2);
    EXPECT_EQ(dfa.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(dfa), 1);
}

// e-замыкание после перехода
TEST_F(DeterminizationTest, HandlesEpsilonClosureAfterMove)
{
    nfa.SetStartState(0);
    nfa.AddFinalState(3);
    nfa.AddTransition(0, 'a', 1);
    nfa.AddTransition(1, EPSILON, 2);
    nfa.AddTransition(2, EPSILON, 3);

    Automaton dfa = DeterminizationAlgorithm::Determine(nfa);

    EXPECT_EQ(dfa.GetStates().size(), 2);
    EXPECT_EQ(dfa.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(dfa), 1);
}

// e-цикл (проверка на бесконечный цикл в EpsilonClosure)
TEST_F(DeterminizationTest, HandlesEpsilonLoop)
{
    nfa.SetStartState(0);
    nfa.AddFinalState(2);
    nfa.AddTransition(0, EPSILON, 1);
    nfa.AddTransition(1, EPSILON, 0);
    nfa.AddTransition(1, 'a', 2);

    Automaton dfa = DeterminizationAlgorithm::Determine(nfa);

    EXPECT_EQ(dfa.GetStates().size(), 2);
    EXPECT_EQ(dfa.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(dfa), 1);
}

// e-замыкание из старта делает его финальным
TEST_F(DeterminizationTest, HandlesEpsilonToFinalFromStart)
{
    nfa.SetStartState(0);
    nfa.AddFinalState(1);
    nfa.AddTransition(0, EPSILON, 1);

    Automaton dfa = DeterminizationAlgorithm::Determine(nfa);

    EXPECT_EQ(dfa.GetStates().size(), 1);
    EXPECT_EQ(dfa.GetFinalStates().size(), 1);
    EXPECT_EQ(GetTransitionCount(dfa), 0);
}

// Сложный смешанный случай (a|b)*ab
TEST_F(DeterminizationTest, HandlesComplexMixedAutomaton)
{
    nfa.SetStartState(0);
    nfa.AddFinalState(2);

    nfa.AddTransition(0, 'a', 0);
    nfa.AddTransition(0, 'b', 0);
    nfa.AddTransition(0, 'a', 1);
    nfa.AddTransition(1, 'b', 2);

    nfa.AddTransition(0, EPSILON, 3);
    nfa.AddTransition(3, 'a', 4);
    nfa.AddTransition(4, EPSILON, 2);

    Automaton dfa = DeterminizationAlgorithm::Determine(nfa);

    EXPECT_EQ(dfa.GetStates().size(), 3);
    EXPECT_EQ(dfa.GetFinalStates().size(), 2);
    EXPECT_EQ(GetTransitionCount(dfa), 6);
}