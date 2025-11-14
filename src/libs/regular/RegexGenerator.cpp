#include "RegexGenerator.h"

#include <random>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace
{
    class GeneratorImpl
    {
    public:
		explicit GeneratorImpl(const std::string& regex)
            : m_regex(regex)
        {
        }

    	std::string GenerateString()
        {
            try
            {
                size_t index = 0;
                std::string result = ParseExpression(index);

                if (index != m_regex.length())
                {
                    throw std::runtime_error("Invalid regex structure or mismatched parentheses");
                }

                return result;
            }
            catch (const std::exception& e)
            {
                std::string errorMsg = "[Error: ";
                errorMsg += e.what();
                errorMsg += "]";
                return errorMsg;
            }
        }

    private:
        std::string ParseExpression(size_t& index)
        {
            std::vector<std::string> parts;
            parts.push_back(ParseTerm(index));

            while (index < m_regex.length() && m_regex[index] == '|')
            {
                index++;
                parts.push_back(ParseTerm(index));
            }

            if (parts.size() > 1)
            {
                int choice = GetRandomInt(0, static_cast<int>(parts.size()) - 1);
                return parts[choice];
            }

            return parts[0];
        }

        std::string ParseTerm(size_t& index)
        {
            std::stringstream ss;
            while (index < m_regex.length() && m_regex[index] != '|' && m_regex[index] != ')')
            {
                ss << ParseFactor(index);
            }
            return ss.str();
        }

        std::string ParseFactor(size_t& index)
        {
            const std::string atom = ParseAtom(index);

            int minRepeats = 1;
            int maxRepeats = 1;
            bool hasQuantifier = false;

            if (index < m_regex.length())
            {
                if (m_regex[index] == '*')
                {
                    minRepeats = 0;
                    maxRepeats = MAX_REPEATS;
                    hasQuantifier = true;
                }
                else if (m_regex[index] == '+')
                {
                    minRepeats = 1;
                    maxRepeats = MAX_REPEATS;
                    hasQuantifier = true;
                }
            }

            if (!hasQuantifier)
            {
                return atom;
            }

            index++;

            const int repeats = GetRandomInt(minRepeats, maxRepeats);
            std::stringstream ss;
            for (int i = 0; i < repeats; ++i)
            {
                ss << atom;
            }
            return ss.str();
        }

        std::string ParseAtom(size_t& index)
        {
            if (index >= m_regex.length())
            {
                throw std::runtime_error("Unexpected end of expression, expected atom");
            }

            if (m_regex[index] == '(')
            {
                index++;
                std::string result = ParseExpression(index);

                if (index >= m_regex.length() || m_regex[index] != ')')
                {
                    throw std::runtime_error("Mismatched opening parenthesis");
                }

                index++;
                return result;
            }

            if (m_regex[index] == '|' || m_regex[index] == ')' || m_regex[index] == '*' || m_regex[index] == '+')
            {
                throw std::runtime_error("Unexpected operator, expected literal or '('");
            }

        	if (m_regex[index] == 'e')
        	{
        		index++;
        		return "";
        	}

            return std::string(1, m_regex[index++]);
        }

        int GetRandomInt(int min, int max) noexcept
        {
            std::uniform_int_distribution dist(min, max);
            return dist(m_rng);
        }

        static constexpr int MAX_REPEATS = 5;

        std::string m_regex;
        std::mt19937 m_rng{std::random_device{}()};
    };
} // namespace

std::vector<std::string> GenerateRegexMatches(const std::string& regex, size_t count)
{
    std::vector<std::string> results;
	if (regex.empty())
    {
        for (size_t i = 0; i < count; ++i)
        {
            results.push_back("");
        }
        return results;
    }

    GeneratorImpl generator(regex);
    for (size_t i = 0; i < count; ++i)
    {
        results.push_back(generator.GenerateString());
    }

    return results;
}