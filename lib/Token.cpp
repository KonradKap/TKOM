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
    switch (input) {
        case Token::identifier:
            return "identifier";
        case Token::integer_value:
            return "integer_value";
        case Token::comment:
            return "#";
        case Token::int_keyword:
            return "int";
        case Token::struct_keyword:
            return "struct";
        case Token::union_keyword:
            return "union";
        case Token::case_keyword:
            return "case";
        case Token::default_keyword:
            return "default";
        case Token::round_bracket_begin:
            return "(";
        case Token::round_bracket_end:
            return ")";
        case Token::square_bracket_begin:
            return "[";
        case Token::square_bracket_end:
            return "]";
        case Token::curly_bracket_begin:
            return "{";
        case Token::curly_bracket_end:
            return "}";
        case Token::semicolon:
            return ";";
        case Token::invalid:
            return "invalid";
        default:
            throw InvalidTokenException("Token not found");
    }
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
