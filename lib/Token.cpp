#include "Token.h"

#include <map>

#include "utility.h"

Token tokenFromString(const std::string& input) {
    static const std::map<std::string, Token> tokens {
        { "#" , Token::comment },
        { "int", Token::int_keyword },
        { "struct", Token::struct_keyword },
        { "union", Token::union_keyword },
        { "case", Token::case_keyword },
        { "default", Token::default_keyword },
        { "(", Token::round_bracket_begin },
        { ")", Token::round_bracket_end },
        { "[", Token::square_bracket_begin },
        { "]", Token::square_bracket_end },
        { "{", Token::curly_bracket_begin },
        { "}", Token::curly_bracket_end },
        { ";", Token::semicolon },
        { ".", Token::dot },
    };

    const auto token = tokens.find(input);
    if (token != tokens.end())
        return token->second;

    if (isValidIdentifier(input))
        return Token::identifier;

    if (isNumericConstant(input))
        return Token::integer_value;

    return Token::invalid;
}

bool isValidToken(const std::string& input) {
    return tokenFromString(input) != Token::invalid;
}

std::string toString(Token input) {
    const static std::vector<std::string> data {
        "identifier",
        "integer",
        "comment",
        "int",
        "struct",
        "union",
        "case",
        "default",
        "{",
        "}",
        "(",
        ")",
        "[",
        "]",
        ";",
        ".",
        "invalid",
    };
    return data[static_cast<decltype(data)::size_type>(input)];
}

std::vector<Token> squareBrackets() {
    return {Token::square_bracket_begin, Token::square_bracket_end};
}

std::vector<Token> roundBrackets() {
    return {Token::round_bracket_begin, Token::round_bracket_end};
}

std::vector<Token> curlyBrackets() {
    return {Token::curly_bracket_begin, Token::curly_bracket_end};
}

std::ostream& operator<<(std::ostream& stream, Token t) {
    return stream << toString(t);
}

std::istream& operator>>(std::istream& stream, Token& t) {
    if (stream.eof())
        throw std::istream::failure("Unexpected end of stream");
    std::string tmp;
    stream >> tmp;
    t = tokenFromString(tmp);
    return stream;
}
