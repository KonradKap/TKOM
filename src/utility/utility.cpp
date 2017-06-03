#include "utility.h"

#include <algorithm>

bool isValidIdentifierStart(int character) {
    return isalpha(character) or character == '_';
}

bool isValidIdentifierContinuation(int character) {
    return isValidIdentifierStart(character) or isdigit(character);
}

bool isValidIdentifier(const std::string& input) {
    return not input.empty()
        and isValidIdentifierStart(input[0])
        and std::all_of(input.begin() + 1, input.end(), isValidIdentifierContinuation);
}

bool isNumericConstant(const std::string& input) {
    return not input.empty() and std::all_of(input.begin(), input.end(), isdigit);
}
