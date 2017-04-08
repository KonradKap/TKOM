#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <exception>

enum class Token {
    identifier,
    integer_value,
    comment,
    int_keyword,
    struct_keyword,
    union_keyword,
    case_keyword,
    default_keyword,
    curly_bracket_begin,
    curly_bracket_end,
    round_bracket_begin,
    round_bracket_end,
    square_bracket_begin,
    square_bracket_end,
    semicolon,
    dot,
    invalid,
    COUNT
};

class InvalidTokenException : public std::invalid_argument {
    public:
        explicit InvalidTokenException(const char *what) : std::invalid_argument(what) {
        }
        explicit InvalidTokenException(const std::string& what) : std::invalid_argument(what) {
        }
};

Token tokenFromString(const std::string& input);
bool isValidToken(const std::string& input);
std::string toString(Token input);

std::vector<Token> squareBrackets();
std::vector<Token> roundBrackets();
std::vector<Token> curlyBrackets();

std::ostream& operator<<(std::ostream& stream, Token t);
std::istream& operator>>(std::istream& stream, Token& t);
