#include "Parser.h"

#include <fstream>
#include <algorithm>
#include <functional>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/range/irange.hpp>

#include "add_containers.h"
#include "utility.h"

Parser::Parser(std::string datafilename, std::string binfilename) :
    scanner(std::make_unique<std::fstream>(datafilename, std::ios_base::in)),
    binaryReader(std::make_unique<std::fstream>(binfilename, std::ios_base::in | std::ios_base::binary)),
    root() {
}

void Parser::parse() {
    parse_data(root);
}

void Parser::signal_error(std::string what) const {
    ::signal_error<ParserException>(scanner, what);
}

void Parser::readTokens(std::vector<Token>::const_iterator& begin, int count) {
    for (const auto _ : boost::irange(0, count)) {
        std::ignore = _;
        const auto token = scanner.getNextToken();
        if (token != *begin)
            signal_error("Expected '" + toString(*begin) + "'");
        ++begin;
    }
}

void Parser::parse_data(Type& parent) {
    if (not parse_structid(parent, false)
        and not parse_unionid(parent, false)
        and not parse_declaration(parent)) {
            std::ignore = scanner.getNextToken();
            signal_error("Expected 'struct', 'union', 'int' or identifier");
    }
    if (not scanner.endOfFile() and scanner.peekNextToken() != Token::curly_bracket_end) {
        parse_data(parent);
    }
}

bool Parser::parse_structid(Type& parent, bool redeclaration) {
    if (scanner.peekNextToken() != Token::struct_keyword)
        return false;

    const auto declaration = scanner.getPosition();
    const std::vector<Token> expectedOrder{
        Token::struct_keyword,
        Token::identifier,
        Token::semicolon,
    };
    auto it = expectedOrder.begin();
    readTokens(it, 2);

    Type new_type;
    new_type.declaration = declaration;
    new_type.type_name = scanner.getLastRead();

    if (not redeclaration and parent.wasDeclared(new_type.type_name))
        signal_error("Identifier: " + new_type.type_name + " was already used");

    new_type.parent = &parent;
    parent.types.push_back(new_type);

    expectDataBlock(parent.types.back());

    handle_redeclaration(redeclaration, parent, new_type);

    readTokens(it, 1);

    return true;
}

void Parser::copy_type_n(int count, Type& parent, const Type& original) {
    for (const auto _ : boost::irange(0, count)) {
        std::ignore = _;
        copy_type(original, parent);
    }
}

void Parser::handle_redeclaration(bool redeclaration, Type& parent, const Type& original) {
    if (redeclaration)
        discard_array_and_identifier();
    else
        parse_array_and_identifier(parent, original);
}

void Parser::parse_array_and_identifier(Type& parent, const Type& original) {
    const auto result = checkForIntegerValue(parent, squareBrackets());
    parent.types.back().identifier = checkForIdentifier(parent);
    copy_type_n(result.first ? result.second : 0, parent, original);
}

void Parser::discard_array_and_identifier() {
    Type discarded;
    std::ignore = checkForIntegerValue(discarded, squareBrackets());
    if (scanner.peekNextToken() == Token::identifier)
        std::ignore = scanner.getNextToken();
}

int64_t Parser::readValue(const Type& context) {
    const auto identifier = scanner.readDeepIdentifier();
    auto* parent = context.findTypeByIdentifier(identifier.begin(), identifier.end() - 1);
    if (parent == nullptr)
        parent = &context;
    const auto* potential_primitive = parent->findPrimitiveByIdentifier({identifier.back()});
    if(potential_primitive) {
        const auto result = checkForIntegerValue(context, squareBrackets());
        if (not result.first and potential_primitive->value.size() > 1)
            signal_error("Expected index in square brackets");
        return potential_primitive->value[result.second];
    } else {
        const auto* type = parent->findTypeByIdentifier({identifier.back()});
        if (not type)
            signal_error("Couldn't find " + identifier.back());
        const auto index = expectIntegerValue(context, squareBrackets());
        const auto* actual_element = type->next(index);
        if (not actual_element)
            signal_error("Overflow error");
        if (scanner.getNextToken() != Token::dot)
            signal_error("Expected '.'");
        return readValue(*actual_element);
    }
}

std::pair<bool, int64_t> Parser::checkForIntegerValue(const Type& context, const std::vector<Token>& brackets) {
    if (scanner.peekNextToken() != brackets.front())
        return {false, 0};

    auto it = brackets.begin();
    readTokens(it, 1);

    const auto token = scanner.peekNextToken();
    if (token != Token::integer_value and token != Token::identifier)
        signal_error("Expected identifier or value");
    const auto data = token == Token::integer_value
        ? scanner.getNextToken(), boost::lexical_cast<int64_t>(scanner.getLastRead())
        : readValue(context);
    readTokens(it, 1);
    return {true, data};
}

int64_t Parser::expectIntegerValue(const Type& context, const std::vector<Token>& brackets) {
    const auto result = checkForIntegerValue(context, brackets);
    if (not result.first)
        signal_error("Expected value");
    return result.second;
}

void Parser::expectDataBlock(Type& parent) {
    const auto brackets = curlyBrackets();
    auto it = brackets.begin();
    readTokens(it, 1);
    parse_data(parent);
    readTokens(it, 1);
}

bool Parser::parse_case(int decider, Type& parent) {
    if (scanner.peekNextToken() != Token::case_keyword)
        signal_error("Expected 'case'");
    std::ignore = scanner.getNextToken();

    const auto value = expectIntegerValue(parent, roundBrackets());
    if (value == decider)
        expectDataBlock(parent);
    else
        discard_datablock();

    return value == decider;
}

void Parser::discard_datablock() {
    const auto saved = std::make_tuple(binaryReader.getOffset(), binaryReader.getPosition());
    Type discard;
    expectDataBlock(discard);
    binaryReader.setOffset(std::get<0>(saved));
    binaryReader.setPosition(std::get<1>(saved));
}

void Parser::parse_caseblock(int decider, Type& parent) {
    auto found_fitting_case = false;
    while (scanner.peekNextToken() == Token::case_keyword) {
        found_fitting_case |= parse_case(decider, parent);
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
        discard_datablock();
    }

}

bool Parser::parse_unionid(Type& parent, bool redeclaration) {
    const auto token = scanner.peekNextToken();
    if (token != Token::union_keyword)
        return false;

    const auto declaration = scanner.getPosition();
    const std::vector<Token> expectedOrder{
        Token::union_keyword,
        Token::identifier,
        Token::curly_bracket_begin,
        Token::curly_bracket_end,
        Token::semicolon
    };
    auto it = expectedOrder.begin();
    readTokens(it, 1);

    const auto decider = expectIntegerValue(parent, roundBrackets());

    readTokens(it, 1);
    const auto type_name = scanner.getLastRead();
    if (not redeclaration and parent.wasDeclared(type_name))
        signal_error("Identifier: " + type_name + " was already used");

    readTokens(it, 1);

    Type new_type;
    new_type.type_name = type_name;
    new_type.parent = &parent;
    new_type.declaration = declaration;
    parent.types.push_back(new_type);
    parse_caseblock(decider, parent.types.back());

    readTokens(it, 1);
    handle_redeclaration(redeclaration, parent, new_type);
    readTokens(it, 1);

    return true;
}

bool Parser::parse_declaration(Type& parent) {
    const auto token = scanner.peekNextToken();
    if (token != Token::int_keyword and token != Token::identifier)
        return false;

    if (token == Token::identifier)
        parse_reuse_of_id(parent);
    else {
        std::ignore = scanner.getNextToken();
        parse_int_declaration(parent);
    }

    if (scanner.getNextToken() != Token::semicolon)
        signal_error("Expected semicolon");

    return true;
}

void Parser::parse_reuse_of_id(Type& parent) {
    const auto type_name = scanner.readDeepIdentifier();
    const auto* original = parent.findTypeByName(type_name);

    if (not original or original->type_name == parent.type_name)
        signal_error(type_name.back() + " was not declared");

    const auto result = checkForIntegerValue(parent, squareBrackets());
    const auto count = result.first
        ? result.second
        : 1;

    const auto identifier = checkForIdentifier(parent);
    auto copy = *original;
    //copy.identifier = identifier;
    //copy_type_n(count, copy, parent);
    for (const auto _ : boost::irange(0l, count)) {
        std::ignore = _;
        copy_type(*original, parent);
        parent.types.back().identifier = identifier;
    }
}

void Parser::copy_type(const Type& original, Type& parent) {
    const auto saved = scanner.getPosition();
    scanner.setPosition(original.declaration);
    if (not parse_structid(parent, true))
        parse_unionid(parent, true);
    scanner.setPosition(saved);
}

void Parser::parse_int_declaration(Type& owner) {
    Primitive new_primitive;
    const auto size_check = checkForIntegerValue(owner, roundBrackets());
    new_primitive.size = size_check.first
        ? size_check.second
        : 32l;
    const auto length_check = checkForIntegerValue(owner, squareBrackets());
    new_primitive.value.resize(length_check.first
        ? length_check.second
        : 1);
    for (auto& val : new_primitive.value)
        val = binaryReader.read(new_primitive.size);

    new_primitive.identifier = checkForIdentifier(owner);
    new_primitive.position = owner.types.size() + owner.primitives.size();
    owner.primitives.push_back(new_primitive);
}

std::string Parser::checkForIdentifier(const Type& context) {
    if (scanner.peekNextToken() != Token::identifier)
        return "";
    std::ignore = scanner.getNextToken();

    const auto identifier = scanner.getLastRead();
    if (context.wasUsed(identifier))
        signal_error(identifier + " was already used");

    return identifier;
}

void Parser::for_each(std::function<void (Type&)> on_type, std::function<void (Primitive&)> on_primitive) {
    root.for_each(on_type, on_primitive);
}

