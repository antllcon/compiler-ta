#pragma once

#include <string>

class RegexRewriter
{
public:
	[[nodiscard]] static std::string Preprocess(const std::string& regex);

private:
	static void ExpandQuantifier(std::string& result, char lastAtom, int min, int max);
	static bool IsDigit(char c);
	static int ParseInt(const std::string& str, size_t& pos);
};