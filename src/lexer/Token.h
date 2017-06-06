#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <utility>
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
    comma,
    eof,
    any,
    COUNT
};

Token tokenFromString(const std::string& input);
bool isValidToken(const std::string& input);
bool isOneLetterToken(char letter);
std::string toString(Token input);

std::pair<Token, Token> squareBrackets();
std::pair<Token, Token> roundBrackets();
std::pair<Token, Token> curlyBrackets();

std::ostream& operator<<(std::ostream& stream, Token t);
std::istream& operator>>(std::istream& stream, Token& t);

bool operator==(Token lhs, Token rhs);
bool operator!=(Token lhs, Token rhs);
bool isAny(Token token);
