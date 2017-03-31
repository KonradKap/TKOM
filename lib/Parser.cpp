#include "Parser.h"

#include <fstream>
#include <algorithm>
#include <functional>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/range/irange.hpp>

#include "add_containers.h"

namespace {
    struct identifier_equality {
        public:
            identifier_equality(const std::string& identifier) :
                identifier(identifier) {
                }

            bool operator()(const auto& t) {
                return t.identifier == identifier;
            }

        private:
            std::string identifier;
    };
}

Parser::Parser(std::string datafilename, std::string binfilename) :
    scanner(std::make_unique<std::fstream>(datafilename, std::ios_base::in)),
    binaryReader(binfilename),
    declared({{}}) {
}

const std::vector<Type>& Parser::parse() {
    parse_data(0);
    return declared;
}

void Parser::signal_error(std::string what) const {
    throw ParserException("Error at line "
            + std::to_string(scanner.getLineNumber())
            + ": "
            + what
            + ", got: '"
            + scanner.getLastRead()
            + "'");
}

void Parser::readTokens(std::vector<Token>::const_iterator& begin, int count) {
    for (auto _ : boost::irange(0, count)) {
        std::ignore = _;
        const auto token = scanner.getNextToken();
        if (token != *begin)
            signal_error("Expected '" + toString(*begin) + "'");
        ++begin;
    }
}

auto Parser::findTypeByIdentifier(const std::string& identifier) const {
    return std::find_if(declared.begin(), declared.end(), identifier_equality(identifier));
}

auto Parser::findTypeByIdentifier(const std::string& identifier) {
    return std::find_if(declared.begin(), declared.end(), identifier_equality(identifier));
}

auto Parser::findPrimitiveByIdentifier(const std::string& identifier) const {
    std::vector<Primitive>::const_iterator primitive;
    const auto type = std::find_if(declared.begin(), declared.end(), [identifier, &primitive](const Type& t) {
            primitive = std::find_if(t.primitives.begin(), t.primitives.end(), identifier_equality(identifier));
            return primitive != t.primitives.end();
        });
    return std::pair<decltype(type), decltype(primitive)>{type, primitive};
}

bool Parser::wasDeclared(const std::string& identifier) const {
    return findTypeByIdentifier(identifier) != declared.end() or findPrimitiveByIdentifier(identifier).first != declared.end();
}

bool Parser::hasValue(const std::string& identifier) const {
    return findPrimitiveByIdentifier(identifier).first != declared.end();
}

bool Parser::isDataStructure(const std::string& identifier) const {
    return findTypeByIdentifier(identifier) != declared.end();
}

void Parser::parse_data(int parent) {
    if (not parse_structid(parent)
        and not parse_unionid(parent)
        and not parse_declaration(parent)) {
            std::ignore = scanner.getNextToken();
            signal_error("Expected 'struct', 'union', 'int' or identifier");
    }
    if (not scanner.endOfFile() and scanner.peekNextToken() != Token::curly_bracket_end) {
        parse_data(parent);
    }
}

bool Parser::parse_structid(int parent) {
    if (scanner.peekNextToken() != Token::struct_keyword)
        return false;

    const std::vector<Token> expectedOrder{
        Token::struct_keyword,
        Token::identifier,
        Token::semicolon,
    };
    auto it = expectedOrder.begin();
    readTokens(it, 2);

    Type new_type {};
    new_type.identifier = scanner.getLastRead();

    if (wasDeclared(new_type.identifier))
        signal_error("Identifier: " + new_type.identifier + " was already used");

    declared.push_back(new_type);
    declared[parent].types.push_back(declared.size() - 1);

    expectDataBlock(declared.size() - 1);

    readTokens(it, 1);

    return true;
}

std::pair<bool, int64_t> Parser::checkForIntegerId(const std::vector<Token>& brackets) {
    if (scanner.peekNextToken() != brackets.front())
        return {false, 0};

    auto it = brackets.begin();
    readTokens(it, 1);

    const auto token = scanner.getNextToken();
    if (token != Token::integer_value and token != Token::identifier)
        signal_error("Expected identifier or value");
    const auto value = scanner.getLastRead();
    int64_t data;
    if (token == Token::integer_value)
        data = boost::lexical_cast<int64_t>(value);
    else {
        const auto primitive = findPrimitiveByIdentifier(scanner.getLastRead());
        if (primitive.first == declared.end())
            signal_error("Expected identifier with value bound to it");
        size_t index=0;
        if (primitive.second->value.size() > 1) {
            const auto result = checkForIntegerId(squareBrackets());
            if (not result.first)
                signal_error("Expected '['");
            index = result.second;
        }
        data = primitive.second->value[index];
    }
    readTokens(it, 1);
    return {true, data};
}

void Parser::expectDataBlock(int parent) {
    const auto brackets = curlyBrackets() + Token::semicolon;
    auto it = brackets.begin();
    readTokens(it, 1);
    parse_data(parent);
    readTokens(it, 1);

}

bool Parser::parse_case(int value, int parent) {
    if (scanner.peekNextToken() != Token::case_keyword)
        signal_error("Expected 'case'");
    std::ignore = scanner.getNextToken();

    const auto range = checkForIntegerId(roundBrackets());
    if (not range.first)
        signal_error("Expected integer value or identifier");

    if (value == range.second)
        expectDataBlock(parent);
    else {
        const auto saved = std::make_tuple(binaryReader.getOffset(), binaryReader.getPosition());
        declared.push_back({});
        expectDataBlock(declared.size() - 1);
        declared.pop_back();
        binaryReader.setOffset(std::get<0>(saved));
        binaryReader.setPosition(std::get<1>(saved));
    }
    return value == range.second;
}

void Parser::parse_caseblock(int value, int parent) {
    auto found_fitting_case = false;
    while (scanner.peekNextToken() == Token::case_keyword) {
        found_fitting_case |= parse_case(value, parent);
    }
    if (not found_fitting_case) {
        if (scanner.peekNextToken() != Token::default_keyword)
            signal_error("Could not find fitting case block");
        std::ignore = scanner.getNextToken();
        expectDataBlock(parent);
        return;
    }
    if (scanner.peekNextToken() == Token::default_keyword) {
        std::ignore = scanner.getNextToken();
        const auto saved = std::make_tuple(binaryReader.getOffset(), binaryReader.getPosition());
        declared.push_back({});
        expectDataBlock(declared.size() - 1);
        declared.pop_back();
        binaryReader.setOffset(std::get<0>(saved));
        binaryReader.setPosition(std::get<1>(saved));
    }

}

bool Parser::parse_unionid(int parent) {
    const auto token = scanner.peekNextToken();
    if (token != Token::union_keyword)
        return false;

    const std::vector<Token> expectedOrder{
        Token::union_keyword,
        Token::identifier,
        Token::curly_bracket_begin,
        Token::curly_bracket_end,
        Token::semicolon
    };
    auto it = expectedOrder.begin();
    readTokens(it, 1);

    const auto range = checkForIntegerId(roundBrackets());
    if (not range.first)
        signal_error("Expected integer value or identifier");

    readTokens(it, 1);
    const auto identifier = scanner.getLastRead();
    readTokens(it, 1);

    Type new_type;
    new_type.identifier = identifier;
    declared.push_back(new_type);
    declared[parent].types.push_back(declared.size() - 1);
    parse_caseblock(range.second, declared.size() - 1);

    readTokens(it, 2);
    return true;
}

bool Parser::parse_declaration(int owner) {
    const auto token = scanner.peekNextToken();
    if (token != Token::int_keyword and token != Token::identifier)
        return false;
    std::ignore = scanner.getNextToken();

    if (token == Token::identifier)
        parse_reuse_of_id(owner);
    else
        parse_int_declaration(owner);

    if (scanner.getNextToken() != Token::semicolon)
        signal_error("Expected semicolon");

    return true;
}

void Parser::parse_reuse_of_id(int owner) {
    const auto type_identifier = scanner.getLastRead();
    const auto result = findTypeByIdentifier(type_identifier);

    if (result == declared.end())
        signal_error(type_identifier + " was not declared");

    const auto check_result = checkForIntegerId(squareBrackets());
    const auto count = check_result.first
        ? check_result.second
        : 1;

    std::string identifier;
    checkForIdentifier(identifier);

    const auto copy = *result;
    for (const auto _ : boost::irange(0l, count)) {
        std::ignore = _;
        auto new_type = copy;
        new_type.identifier = identifier;
        declared.push_back(new_type);
        declared[owner].types.push_back(declared.size() - 1);
        for_each(declared, declared.size() - 1, [](Type& t) {},
            [=](Primitive& p) {
                for(auto& i : p.value)
                    i = binaryReader.read(p.size);
            }
        );
    }
}

void Parser::parse_int_declaration(int owner) {
    Primitive new_primitive;
    const auto size_check = checkForIntegerId(roundBrackets());
    if (size_check.first)
        new_primitive.size = size_check.second;
    const auto length_check = checkForIntegerId(squareBrackets());
    if (length_check.first) {
        new_primitive.value.resize(length_check.second);
    }
    for (auto& val : new_primitive.value)
        val = binaryReader.read(new_primitive.size);

    checkForIdentifier(new_primitive.identifier);
    new_primitive.position = declared[owner].types.size() + declared[owner].primitives.size();
    declared[owner].primitives.push_back(new_primitive);
}

void Parser::checkForIdentifier(auto& data) {
    if (scanner.peekNextToken() != Token::identifier)
        return;
    std::ignore = scanner.getNextToken();

    const auto identifier = scanner.getLastRead();
    if (wasDeclared(identifier))
        signal_error(identifier + " was already used");

    data = identifier;
}

void for_each(
        std::vector<Type>& input, size_t begin,
        std::function<void (Type&)> on_type,
        std::function<void (Primitive&)> on_primitive) {

    for (unsigned prim = 0, subtype = 0; prim + subtype < input[begin].primitives.size() + input[begin].types.size();) {
        if (prim < input[begin].primitives.size() and input[begin].primitives[prim].position == prim + subtype) {
            on_primitive(input[begin].primitives[prim]);
            ++prim;
        }
        else {
            on_type(input[input[begin].types[subtype]]);
            for_each(input, input[begin].types[subtype], on_type, on_primitive);
            ++subtype;
        }
    }
}
