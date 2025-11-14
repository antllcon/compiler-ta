#pragma once

#include <string>
#include <vector>

// Поддерживаемые символы '(', ')', '|', '*', '+'), понимает e-переходы
std::vector<std::string> GenerateRegexMatches(const std::string& regex, size_t count);