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
			// Если это экранирование, сохраняем как есть,
			// но C-движок должен знать, что это литерал.
			// Мы передадим это дальше, а re2post обработает.
			result += c;
			if (i + 1 < regex.length())
			{
				result += regex[++i];
			}
			continue;
		}

		if (c == '{')
		{
			// Начало квантификатора: ищем закрывающую скобку
			// ВАЖНО: Квантификатор относится к предыдущему символу.
			// Если result пуст, это ошибка (или литерал '{', но считаем валидным regex).
			if (result.empty())
			{
				result += c;
				continue;
			}

			// Проверяем, есть ли там числа
			size_t nextPos = i + 1;
			int min = ParseInt(regex, nextPos);
			int max = min; // По дефолту {n} -> min=n, max=n

			bool rangeFound = false;
			if (nextPos < regex.length() && regex[nextPos] == ',')
			{
				rangeFound = true;
				nextPos++;
				if (nextPos < regex.length() && regex[nextPos] == '}')
				{
					// Случай {n,} -> max = бесконечность (-1)
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
				// Успешно распарсили конструкцию {..}
				// Извлекаем последний атом (упрощенно: последний символ)
				// ВНИМАНИЕ: Для полной поддержки группировки (abc){2} нужен более сложный парсер.
				// Сейчас реализуем для одиночных символов, как в базовом NFA.
				char lastAtom = result.back();
				result.pop_back(); // Убираем, чтобы перезаписать развернутую версию

				ExpandQuantifier(result, lastAtom, min, max);

				i = nextPos; // Пропускаем обработанную часть
				continue;
			}
		}

		// Обычный символ
		result += c;
	}

	return result;
}

void RegexRewriter::ExpandQuantifier(std::string& result, char lastAtom, int min, int max)
{
	// 1. Обязательная часть (min раз)
	for (int k = 0; k < min; ++k)
	{
		result += lastAtom;
	}

	// 2. Опциональная часть
	if (max == -1) // {n,} -> n раз + замыкание Клини (*)
	{
		result += lastAtom;
		result += '*';
	}
	else // {n,m}
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