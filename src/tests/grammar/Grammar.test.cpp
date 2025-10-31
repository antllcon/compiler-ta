#include "gtest/gtest.h"
#include <Automaton.h>
#include <Grammar.h>
#include <GrammarToNfaConverter.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
Grammar CreateTestGrammar(
	const SymbolString& startSymbol,
	const std::vector<SymbolString>& nonTerminals,
	const std::vector<SymbolString>& terminals,
	const std::vector<Production>& productions)
{
	Grammar g;
	for (const auto& nt : nonTerminals)
	{
		g.AddNonTerminal(nt);
	}
	for (const auto& t : terminals)
	{
		g.AddTerminal(t);
	}

	g.SetStartSymbol(startSymbol);

	for (const auto& p : productions)
	{
		g.AddProduction(p);
	}
	return g;
}
} // namespace

TEST(GrammarToNfaConverterTest, RightLinear_SingleTerminal)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S"},
		{"a"},
		{{"S", "a"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("aa"));
}

TEST(GrammarToNfaConverterTest, RightLinear_SimpleChain)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "B"},
		{"a", "b"},
		{{"S", "aB"}, {"B", "b"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("ab"));
	EXPECT_FALSE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize(""));
}

TEST(GrammarToNfaConverterTest, RightLinear_SimpleChoice)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S"},
		{"a", "b"},
		{{"S", "a"}, {"S", "b"}}); // S -> a | b

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_TRUE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("ab"));
	EXPECT_FALSE(nfa.Recognize(""));
}

TEST(GrammarToNfaConverterTest, RightLinear_EpsilonStart)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S"},
		{"e"},
		{{"S", "e"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize(""));
	EXPECT_TRUE(nfa.Recognize("e"));
	EXPECT_FALSE(nfa.Recognize("a"));
}

TEST(GrammarToNfaConverterTest, RightLinear_KleeneStar)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S"},
		{"a", "e"},
		{{"S", "aS"}, {"S", "e"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize(""));
	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_TRUE(nfa.Recognize("aaaaa"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("aaab"));
}

TEST(GrammarToNfaConverterTest, RightLinear_EpsilonTransition)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "A", "B"},
		{"a", "b", "e"},
		{{"S", "aA"}, {"A", "eB"}, {"B", "b"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("ab"));
	EXPECT_TRUE(nfa.Recognize("aeb"));
	EXPECT_FALSE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize("b"));
}

TEST(GrammarToNfaConverterTest, RightLinear_Nondeterminism)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "B", "C"},
		{"a", "b", "c"},
		{{"S", "aB"}, {"S", "aC"}, {"B", "b"}, {"C", "c"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("ab"));
	EXPECT_TRUE(nfa.Recognize("ac"));
	EXPECT_FALSE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize("abc"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("c"));
}

TEST(GrammarToNfaConverterTest, RightLinear_MultiCharTerminals)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "A", "B"},
		{"a", "b", "c", "d", "e", "f", "g"},
		{{"S", "abA"}, {"A", "cdeB"}, {"B", "fg"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("abcdefg"));
	EXPECT_FALSE(nfa.Recognize("ab"));
	EXPECT_FALSE(nfa.Recognize("cde"));
	EXPECT_FALSE(nfa.Recognize("fg"));
	EXPECT_FALSE(nfa.Recognize("abcdef"));
}

TEST(GrammarToNfaConverterTest, RightLinear_ComplexLoopAndBranch)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "A"},
		{"a", "b", "c", "d", "f"},
		{{"S", "aS"}, {"S", "bA"}, {"S", "c"}, {"A", "dA"}, {"A", "f"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("c"));
	EXPECT_TRUE(nfa.Recognize("ac"));
	EXPECT_TRUE(nfa.Recognize("aaac"));
	EXPECT_TRUE(nfa.Recognize("bf"));
	EXPECT_TRUE(nfa.Recognize("bdf"));
	EXPECT_TRUE(nfa.Recognize("bdddf"));
	EXPECT_TRUE(nfa.Recognize("abf"));
	EXPECT_TRUE(nfa.Recognize("aabdddf"));

	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("bd"));
	EXPECT_FALSE(nfa.Recognize("ba"));
	EXPECT_FALSE(nfa.Recognize("cf"));
}

TEST(GrammarToNfaConverterTest, RightLinear_ThrowsOnMixed)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "B"},
		{"a"},
		{{"S", "aS"}, {"B", "Sa"}});

	EXPECT_THROW(GrammarToNfaConverter::Convert(g), std::invalid_argument);
}

TEST(GrammarToNfaConverterTest, RightLinear_ThrowsOnNonLinear)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S"},
		{"a", "b"},
		{{"S", "aSb"}}); // Нелинейная

	EXPECT_THROW(GrammarToNfaConverter::Convert(g), std::invalid_argument);
}

TEST(GrammarToNfaConverterTest, LeftLinear_SingleTerminal)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S"},
		{"a"},
		{{"S", "a"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("aa"));
}

TEST(GrammarToNfaConverterTest, LeftLinear_SimpleChain)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "B"},
		{"a", "b"},
		{{"S", "Ba"}, {"B", "b"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("ba"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize("ab"));
}

TEST(GrammarToNfaConverterTest, LeftLinear_UsersDebugExample)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "B"},
		{"a", "c", "x"},
		{{"S", "Ba"}, {"S", "Bac"}, {"B", "x"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("xa"));
	EXPECT_TRUE(nfa.Recognize("xac"));
	EXPECT_FALSE(nfa.Recognize("x"));
	EXPECT_FALSE(nfa.Recognize("xaca"));
	EXPECT_FALSE(nfa.Recognize("ax"));
}

TEST(GrammarToNfaConverterTest, LeftLinear_EpsilonStart)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S"},
		{"e"},
		{{"S", "e"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize(""));
	EXPECT_TRUE(nfa.Recognize("e"));
	EXPECT_FALSE(nfa.Recognize("a"));
}

TEST(GrammarToNfaConverterTest, LeftLinear_EpsilonNonStart)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "B"},
		{"a", "e"},
		{{"S", "Ba"}, {"B", "e"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("ba"));
}

TEST(GrammarToNfaConverterTest, LeftLinear_KleeneStar)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S"},
		{"a", "e"},
		{{"S", "Sa"}, {"S", "e"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize(""));
	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_TRUE(nfa.Recognize("aaaaa"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("aaab"));
}

TEST(GrammarToNfaConverterTest, LeftLinear_MultiCharTerminals)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "B"},
		{"a", "b", "c"},
		{{"S", "Baa"}, {"B", "bc"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("bcaa"));
	EXPECT_FALSE(nfa.Recognize("bc"));
	EXPECT_FALSE(nfa.Recognize("aa"));
	EXPECT_FALSE(nfa.Recognize("aabc"));
}

TEST(GrammarToNfaConverterTest, LeftLinear_ComplexLoopAndBranch)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "A"},
		{"a", "b", "c", "d", "f"},
		{{"S", "Sa"}, {"S", "Ab"}, {"S", "c"}, {"A", "Ad"}, {"A", "f"}});

	Automaton nfa = GrammarToNfaConverter::Convert(g);

	EXPECT_TRUE(nfa.Recognize("c"));
	EXPECT_TRUE(nfa.Recognize("ca"));
	EXPECT_TRUE(nfa.Recognize("caaa"));
	EXPECT_TRUE(nfa.Recognize("fb"));
	EXPECT_TRUE(nfa.Recognize("fba"));
	EXPECT_TRUE(nfa.Recognize("fdb"));
	EXPECT_TRUE(nfa.Recognize("fdddbaa"));

	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("f"));
	EXPECT_FALSE(nfa.Recognize("fd"));
	EXPECT_FALSE(nfa.Recognize("ba"));
	EXPECT_FALSE(nfa.Recognize("cf"));
}

TEST(GrammarToNfaConverterTest, LeftLinear_ThrowsOnMixed)
{
	Grammar g = CreateTestGrammar(
		"S",
		{"S", "B"},
		{"a"},
		{{"S", "Sa"}, {"B", "aB"}});

	EXPECT_THROW(GrammarToNfaConverter::Convert(g), std::invalid_argument);
}