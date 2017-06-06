#include "Parser.h"

#include <fstream>
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>

#include <boost/lexical_cast.hpp>
#include <boost/range/irange.hpp>

#include "utility/utility.h"
#include "utility/print_container.h"
#include "utility/CompilationError.h"
#include "lexer/Scanner.h"
#include "Struct.h"

using namespace std::placeholders;

Parser::Parser(Scanner& scanner) :
    scanner(scanner),
    root(std::make_shared<Struct>()),
    Int(std::make_shared<Integer>(root)) {
}

std::shared_ptr<Type> Parser::parse() {
    try {
        parseData(root);
    } catch (CompilationError& error) {
        if (error.line_number == -1) {
            error.line_number = scanner.getLineNumber();
            error.column_number = scanner.getColumnNumber();
            error.length = scanner.getLastRead().length();
        }
        throw error;
    }
    return root;
}

bool Parser::expectToken(Token expected) {
    if (not checkToken(expected))
        throw CompilationError("Expected '" + toString(expected)
                + "', got: '" + scanner.getLastRead() + "'");
    return true;
}

bool Parser::checkToken(Token expected) {
    if (isAny(expected))
        return true;
    if (scanner.peekNextToken() != expected)
        return false;
    std::ignore = scanner.getNextToken();
    return true;
}

bool Parser::checkToken(const std::vector<Token>& tokens) {
    return std::any_of(tokens.begin(), tokens.end(),
        [this] (Token token) { return checkToken(token); });
}

ReferenceList Parser::parseReferenceList(std::pair<Token, Token> brackets) {
    ReferenceList ret;
    parseSubList(ret, brackets.first, brackets.second);
    return ret;
}

ReferenceList& Parser::parseReferenceList(ReferenceList& list) {
    const ReferenceList::LengthMeasure _{scanner, list};
    expectToken(Token::identifier);
    ReferenceList::Element next_element{{}, scanner.getLastRead()};
    list.addElement(next_element);
    if (checkToken(Token::dot))
        return parseReferenceList(list);
    parseSubList(list.elements.back().list, Token::square_bracket_begin, Token::square_bracket_end);
    if (checkToken(Token::dot))
        return parseReferenceList(list);
    return list;
}

void Parser::parseSubList(ReferenceList& parent, Token begin, Token end) {
    if (checkToken(begin)) {
        if (checkToken(Token::integer_value)) {
            parent.addElement({{}, scanner.getLastRead()});
        }
        else if (scanner.peekNextToken() == Token::identifier)
            parseReferenceList(parent);
        parent.column_number = scanner.getColumnNumber();
        parent.line_number = scanner.getLineNumber();
        expectToken(end);
    }
}

void Parser::parseData(std::shared_ptr<Type> parent) {
    static const std::map<Token, std::function<void(Parser*, std::shared_ptr<Type>)>> parsers {
        { Token::struct_keyword, &Parser::parseStructDeclaration },
        { Token::union_keyword, &Parser::parseUnionDeclaration },
        { Token::int_keyword, &Parser::parseIntegerUsage },
        { Token::identifier, &Parser::parseIdentifierUsage },
        { Token::curly_bracket_end, [](Parser*, std::shared_ptr<Type>){} },
        { Token::eof, [](Parser*, std::shared_ptr<Type>) { throw CompilationError("Unexpected end of file"); } },
    };
    const auto token = scanner.peekNextToken();
    const auto function = parsers.find(token);
    if (function != parsers.end())
        std::bind(function->second, this, parent)();
    else
        throw CompilationError("Token '" + toString(token) + "' was not expected");
    if (not scanner.endOfFile() and scanner.peekNextToken() != Token::curly_bracket_end)
        parseData(parent);
}

void Parser::parseDataBlock(std::shared_ptr<Type> parent) {
    expectToken(Token::curly_bracket_begin);
    parseData(parent);
    expectToken(Token::curly_bracket_end);
    expectToken(Token::semicolon);
}

void Parser::parseStructDeclaration(std::shared_ptr<Type> parent) {
    const auto data = parseDeclarationBegin(parent, Token::struct_keyword);
    auto new_struct = std::make_shared<Struct>(data.second, parent, data.first);
    parseDataBlock(new_struct);
    parent->addSubtype(new_struct);
}

argument_list_t Parser::parseArgumentsList() {
    argument_list_t results;
    expectToken(Token::round_bracket_begin);
    if (checkToken(Token::identifier)) {
        do
            results.push_back(scanner.getLastRead());
        while (checkToken(Token::comma) and expectToken(Token::identifier));
    }
    checkListEnd();
    return results;
}

argument_list_t Parser::checkForArgumentsList(argument_list_t default_value) {
    if (scanner.peekNextToken() == Token::round_bracket_begin)
        return parseArgumentsList();
    return default_value;
}

void Parser::parseUnionDeclaration(std::shared_ptr<Type> parent) {
    const auto data = parseDeclarationBegin(parent, Token::union_keyword);
    auto new_union = std::make_shared<Union>(data.second, parent, data.first);
    parseUnionBody(new_union);
    expectToken(Token::semicolon);
    parent->addSubtype(new_union);
}

void Parser::parseUnionBody(std::shared_ptr<Union> new_union) {
    expectToken(Token::curly_bracket_begin);
    checkCases(new_union);
    checkDefault(new_union);
    expectToken(Token::curly_bracket_end);
}

void Parser::checkCases(std::shared_ptr<Union> new_union) {
    while (checkToken(Token::case_keyword)) {
        const auto refrence_list = parseReferenceList(roundBrackets());
        auto case_block = std::make_shared<Struct>("", new_union);
        parseDataBlock(case_block);
        new_union->cases.push_back({refrence_list, case_block});
    }
}

void Parser::checkDefault(std::shared_ptr<Union> new_union) {
    if (checkToken(Token::default_keyword)) {
        new_union->default_case = std::make_shared<Struct>("", new_union);
        parseDataBlock(new_union->default_case);
    }
}

std::pair<argument_list_t, std::string>
Parser::parseDeclarationBegin(std::shared_ptr<Type> parent, Token begin) {
    expectToken(begin);
    const auto arguments_list = checkForArgumentsList();
    expectToken(Token::identifier);
    const auto identifier = scanner.getLastRead();
    if (identifier != "" and parent->wasDeclared(identifier))
        throw CompilationError("Identifier '" + identifier + "' was already used");
    return {arguments_list, identifier};
}

void Parser::parseIdentifierUsage(std::shared_ptr<Type> parent) {
    if (scanner.peekNextToken() != Token::identifier)
        throw CompilationError("Expected identifier");
    const auto identifier = scanner.readDeepIdentifier();
    const auto type = parent->findSubtype(identifier);
    if (type == nullptr)
        throw CompilationError("Failed to find '" + toString(identifier, ".") + "'");
    parseUsage(parent, type);
}

void Parser::parseIntegerUsage(std::shared_ptr<Type> parent) {
    expectToken(Token::int_keyword);
    parseUsage(parent, Int);
}

void Parser::parseUsage(std::shared_ptr<Type> parent, std::shared_ptr<Type> source) {
    const auto arguments = checkForArguments(source->defaultArguments());
    if (arguments.size() != source->argumentCount())
        throw CompilationError("Wrong number of arguments: " + std::to_string(arguments.size())
                + ", expected: " + std::to_string(source->argumentCount()));
    const auto count = checkForArray(DeclaredData::defaultCount);
    const auto identifier = checkForIdentifier();
    if (identifier != "" and parent->wasUsed(identifier))
        throw CompilationError("Identifier '" + identifier + "' was used");
    expectToken(Token::semicolon);
    parent->declared.push_back({source, identifier, count, arguments});
}

void Parser::checkListEnd() {
    if (not checkToken(Token::round_bracket_end)) {
        std::ignore = scanner.getNextToken();
        throw CompilationError("Expected ')' or ',', got: '" + scanner.getLastRead() + "'");
    }
}

std::vector<ReferenceList> Parser::checkForArguments(const std::vector<ReferenceList>& default_value) {
    if (checkToken(Token::round_bracket_begin)
            and scanner.peekNextToken() != Token::round_bracket_end) {
        std::vector<ReferenceList> ret;
        checkForArguments(ret, 0);
        checkListEnd();
        return ret;
    }
    std::ignore = checkToken(Token::round_bracket_end);
    return default_value;
}

void Parser::checkForArguments(std::vector<ReferenceList>& ret, int) {
    ret.push_back(parseReferenceList({Token::any, Token::any}));
    if (checkToken(Token::comma))
        checkForArguments(ret, 0);
}

ReferenceList Parser::checkForArray(const ReferenceList& default_value) {
    if (scanner.peekNextToken() == Token::square_bracket_begin)
        return parseReferenceList(squareBrackets());
    return default_value;
}

std::string Parser::checkForIdentifier(const std::string& default_value) {
    if (checkToken(Token::identifier))
        return scanner.getLastRead();
    return default_value;
}
