#include "RegexRewriter.h"
#include <limits>

std::string RegexRewriter::Preprocess(const std::string& regex)
{
	std::string result;
	result.reserve(regex.length() * 2);

	for (size_t i = 0; i < regex.length(); ++i)
	{
		char c = regex[i];

		if (c == '\\')
		{
			result += c;
			if (i + 1 < regex.length())
			{
				result += regex[++i];
			}
			continue;
		}

		if (c == '{')
		{
			if (result.empty())
			{
				result += c;
				continue;
			}

			size_t nextPos = i + 1;
			int min = ParseInt(regex, nextPos);
			int max = min;

			bool rangeFound = false;
			if (nextPos < regex.length() && regex[nextPos] == ',')
			{
				rangeFound = true;
				nextPos++;
				if (nextPos < regex.length() && regex[nextPos] == '}')
				{
					max = -1;
				}
				else
				{
					// Случай {n,m}
					max = ParseInt(regex, nextPos);
				}
			}

			if (nextPos < regex.length() && regex[nextPos] == '}')
			{
				char lastAtom = result.back();
				result.pop_back();

				ExpandQuantifier(result, lastAtom, min, max);

				i = nextPos;
				continue;
			}
		}

		result += c;
	}

	return result;
}

void RegexRewriter::ExpandQuantifier(std::string& result, char lastAtom, int min, int max)
{
	for (int k = 0; k < min; ++k)
	{
		result += lastAtom;
	}

	if (max == -1)
	{
		result += lastAtom;
		result += '*';
	}
	else
	{
		int count = max - min;
		for (int k = 0; k < count; ++k)
		{
			result += lastAtom;
			result += '?';
		}
	}
}

bool RegexRewriter::IsDigit(char c)
{
	return c >= '0' && c <= '9';
}

int RegexRewriter::ParseInt(const std::string& str, size_t& pos)
{
	int val = 0;
	while (pos < str.length() && IsDigit(str[pos]))
	{
		val = val * 10 + (str[pos] - '0');
		pos++;
	}
	return val;
}