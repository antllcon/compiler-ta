#pragma once

#include "Lexer.h"
#include "../automaton/Automaton.h"

#include <cctype>
#include <string_view>

Lexer::Lexer(std::string_view input)
	: m_input(input)
{
}

char Lexer::Peek() const noexcept
{
	return m_pos < m_input.size() ? m_input[m_pos] : '\0';
}

char Lexer::Get() noexcept
{
	if (m_pos < m_input.size())
		return m_input[m_pos++];
	return '\0';
}

void Lexer::SkipWhitespace() noexcept
{
	while (std::isspace(static_cast<unsigned char>(Peek())))
		++m_pos;
}

Token Lexer::Next()
{
	SkipWhitespace();
	const size_t startPos = m_pos;
	char c = Peek();

	if (c == '\0')
		return {TokenKind::EndOfFile, "", m_pos};

	// Single-char punct
	switch (c)
	{
	case '{':
		Get();
		return {TokenKind::LBrace, "{", startPos};
	case '}':
		Get();
		return {TokenKind::RBrace, "}", startPos};
	case '(':
		Get();
		return {TokenKind::LParen, "(", startPos};
	case ')':
		Get();
		return {TokenKind::RParen, ")", startPos};
	case '+':
		Get();
		return {TokenKind::Plus, "+", startPos};
	case '=':
		Get();
		return {TokenKind::Assign, "=", startPos};
	case '>': {
		Get();
		if (Peek() == '=')
		{
			Get();
			return {TokenKind::GreaterEq, ">=", startPos};
		}
		return {TokenKind::Greater, ">", startPos};
	}
	case '<': {
		Get();
		if (Peek() == '=')
		{
			Get();
			return {TokenKind::LessEq, "<=", startPos};
		}
		return {TokenKind::Less, "<", startPos};
	}
	default:
		break;
	}

	// Identifier or number
	if (std::isalpha(static_cast<unsigned char>(c)) || c == '_')
		return MakeIdentifierOrKeyword(startPos);

	if (std::isdigit(static_cast<unsigned char>(c)))
		return MakeNumber(startPos);

	// Unknown
	Get();
	return {TokenKind::Unknown, std::string(1, c), startPos};
}

void Lexer::SetText(const std::string_view text)
{
	m_input = text;
}

Token Lexer::MakeIdentifierOrKeyword(size_t startPos)
{
	while (std::isalnum(static_cast<unsigned char>(Peek())) || Peek() == '_')
		Get();

	std::string text{m_input.substr(startPos, m_pos - startPos)};

	if (text == "bol") return {TokenKind::Bol, text, startPos};
	if (text == "stg") return {TokenKind::Stg, text, startPos};
	if (text == "int") return {TokenKind::Int, text, startPos};
	if (text == "lng") return {TokenKind::Lng, text, startPos};
	if (text == "chr") return {TokenKind::Chr, text, startPos};
	if (text == "dbl") return {TokenKind::Dbl, text, startPos};
	if (text == "flt") return {TokenKind::Flt, text, startPos};

	if (text == "return") return {TokenKind::Return, text, startPos};
	if (text == "println") return {TokenKind::Println, text, startPos};
	if (text == "print") return {TokenKind::Print, text, startPos};
	if (text == "main") return {TokenKind::Main, text, startPos};

	return {TokenKind::Identifier, text, startPos};
}

Token Lexer::MakeNumber(size_t startPos)
{
	bool hasDot = false;

	while (true)
	{
		char c = Peek();
		if (std::isdigit(static_cast<unsigned char>(c)))
		{
			Get();
			continue;
		}
		if (c == '.' && !hasDot)
		{
			hasDot = true;
			Get();
			continue;
		}
		break;
	}

	std::string text{m_input.substr(startPos, m_pos - startPos)};
	return hasDot ? Token{TokenKind::FloatLiteral, text, startPos}
				  : Token{TokenKind::IntLiteral, text, startPos};
}
