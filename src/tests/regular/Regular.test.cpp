#include "gtest/gtest.h"
#include "RegexConverterAdapter.h"
#include "Automaton.h"

// (Этот тест проверяет базовый символ)
TEST(RegexConverterAdapterTest, LiteralCharacter)
{
	Automaton nfa;
	std::string regex = "a";

	// 1. Конвертация
	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_TRUE(success);

	// 2. Проверка
	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("aa"));
}

TEST(RegexConverterAdapterTest, Concatenation)
{
	Automaton nfa;
	std::string regex = "ab"; // C-API неявно добавляет '.'

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_TRUE(success);

	EXPECT_TRUE(nfa.Recognize("ab"));
	EXPECT_FALSE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("ba"));
	EXPECT_FALSE(nfa.Recognize("abc"));
}

TEST(RegexConverterAdapterTest, Union)
{
	Automaton nfa;
	std::string regex = "a|b";

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_TRUE(success);

	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_TRUE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("c"));
	EXPECT_FALSE(nfa.Recognize("ab"));
}

TEST(RegexConverterAdapterTest, StarOperator)
{
	Automaton nfa;
	std::string regex = "a*";

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_TRUE(success);

	// * (ноль или более)
	EXPECT_TRUE(nfa.Recognize(""));
	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_TRUE(nfa.Recognize("aaaaa"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("ab"));
	EXPECT_FALSE(nfa.Recognize("ba"));
}

TEST(RegexConverterAdapterTest, PlusOperator)
{
	Automaton nfa;
	std::string regex = "a+"; // C-API поддерживает '+'

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_TRUE(success);

	// + (один или более)
	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_TRUE(nfa.Recognize("aaaaa"));
	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("ab"));
}

TEST(RegexConverterAdapterTest, QuestionOperator)
{
	Automaton nfa;
	std::string regex = "a?"; // C-API поддерживает '?'

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_TRUE(success);

	// ? (ноль или один)
	EXPECT_TRUE(nfa.Recognize(""));
	EXPECT_TRUE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize("aa"));
	EXPECT_FALSE(nfa.Recognize("b"));
}

TEST(RegexConverterAdapterTest, ComplexCombinationWithGrouping)
{
	Automaton nfa;
	// Ноль или более 'a' или 'b', за которыми следует 'c'
	std::string regex = "(a|b)*c";

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_TRUE(success);

	EXPECT_TRUE(nfa.Recognize("c"));
	EXPECT_TRUE(nfa.Recognize("ac"));
	EXPECT_TRUE(nfa.Recognize("bc"));
	EXPECT_TRUE(nfa.Recognize("abc"));
	EXPECT_TRUE(nfa.Recognize("babac"));
	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("acb"));
}

TEST(RegexConverterAdapterTest, NestedGroupingAndPlus)
{
	Automaton nfa;
	std::string regex = "(a(b|c)+)+";

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_TRUE(success);

	EXPECT_TRUE(nfa.Recognize("ab"));
	EXPECT_TRUE(nfa.Recognize("ac"));
	EXPECT_TRUE(nfa.Recognize("abbc"));
	EXPECT_TRUE(nfa.Recognize("abac"));
	EXPECT_TRUE(nfa.Recognize("acccab"));
	EXPECT_FALSE(nfa.Recognize(""));
	EXPECT_FALSE(nfa.Recognize("a"));
	EXPECT_FALSE(nfa.Recognize("b"));
	EXPECT_FALSE(nfa.Recognize("abx"));
}

TEST(RegexConverterAdapterTest, InvalidRegexUnbalancedParen)
{
	Automaton nfa;
	// re2post вернет NULL
	std::string regex = "(a|b";

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_FALSE(success);
}

TEST(RegexConverterAdapterTest, InvalidRegexLeadingOperator)
{
	Automaton nfa;
	// re2post вернет NULL
	std::string regex = "*a";

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_FALSE(success);
}

TEST(RegexConverterAdapterTest, InvalidRegexEmptyParen)
{
	Automaton nfa;
	// re2post вернет NULL (natom == 0 при ')' )
	std::string regex = "()";

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_FALSE(success);
}

TEST(RegexConverterAdapterTest, EmptyRegex)
{
	Automaton nfa;
	// re2post вернет "", post2nfa вернет NULL
	std::string regex = "";

	bool success = RegexConverterAdapter::Convert(regex, nfa);
	ASSERT_FALSE(success);
}