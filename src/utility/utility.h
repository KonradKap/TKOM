#pragma once

#include <string>
#include <exception>

bool isValidIdentifierStart(int character);
bool isValidIdentifierContinuation(int character);
bool isValidIdentifier(const std::string& input);
bool isNumericConstant(const std::string& input);
