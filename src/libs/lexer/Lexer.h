#pragma once

#include <string>
#include <string_view>

enum class TokenKind
{
	EndOfFile,
	Unknown,

	// Data types
	Bol,
	Stg,
	Int,
	Lng,
	Chr,
	Dbl,
	Flt,

	// Identifiers and literals
	Identifier,
	IntLiteral,
	FloatLiteral,
	StringLiteral,
	CharLiteral,

	// Punctuation
	LBrace, // {
	RBrace, // }
	LParen, // (
	RParen, // )

	// Keywords
	Return,
	Println,
	Print,
	Main,

	// Operators
	Assign,	  // =
	Plus,	  // +
	Greater,  // >
	Less,	  // <
	LessEq,	  // <=
	GreaterEq // >=
};

struct Token
{
	TokenKind kind{TokenKind::Unknown};
	std::string value;
	size_t position;
};

class Lexer
{
public:
	Lexer() = default;
	explicit Lexer(std::string_view input);

	Token Next();
	void SetText(std::string_view text);

private:
	std::string_view m_input;
	size_t m_pos = 0;

	char Peek() const noexcept;
	char Get() noexcept;
	void SkipWhitespace() noexcept;

	Token MakeIdentifierOrKeyword(size_t startPos);
	Token MakeNumber(size_t startPos);
};
