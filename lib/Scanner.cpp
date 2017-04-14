#include "Scanner.h"

#include <limits>
#include <tuple>
#include <functional>
#include <map>

#include "utility.h"

Scanner::Scanner(std::unique_ptr<std::istream> input) :
    input(std::move(input)),
    line_number(0),
    column_number(0),
    last_read() {

}

Token Scanner::getNextToken() {
    ignoreWhitespacesAndComments();
    if (endOfFile())
        throw std::istream::failure("Unexpected end of stream");
    last_read = "";
    while (*input and not (isValidToken(last_read) and isLongestMatch()))
        last_read += static_cast<char>(input->get());

    column_number += last_read.length();
    const auto token = tokenFromString(last_read);

    return token;
}

Token Scanner::peekNextToken() {
    const auto saved = std::make_tuple(last_read, column_number, line_number, input->tellg());
    ignoreWhitespacesAndComments();
    const auto token = endOfFile()
        ? Token::invalid
        : getNextToken();

    std::tie(last_read, column_number, line_number, std::ignore) = saved;
    input->seekg(std::get<3>(saved));
    return token;
}

std::vector<std::string> Scanner::readDeepIdentifier() {
    if (peekNextToken() != Token::identifier)
        return {};

    std::ignore = getNextToken();
    std::vector<std::string> ret{ getLastRead() };
    if (peekNextToken() == Token::dot)
        do {
            std::ignore = getNextToken();
            if (getNextToken() != Token::identifier)
                throw std::invalid_argument("Expected identifier, got : '" + getLastRead() + "'");
            ret.push_back(getLastRead());
        } while (peekNextToken() == Token::dot);
    return ret;
}

void Scanner::readUntill(Token stop) {
    while(peekNextToken() != stop)
        getNextToken();
}

bool Scanner::isLongestMatch() const {
    return isValidToken(last_read) and not isValidToken(last_read + static_cast<char>(input->peek()));
}

void Scanner::ignoreWhitespacesAndComments() {
    while (*input and (isspace(input->peek()) or input->peek() == '#')) {
        const auto character = input->get();
        if (character == '#')
            ignoreComments();
        else if (character == '\n')
            nextLine();
        else
            ++column_number;
    }
}

void Scanner::ignoreComments() {
    input->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    ++line_number;
    column_number = 0;
}

void Scanner::nextLine() {
    column_number = 0;
    ++line_number;
}

bool Scanner::endOfFile() {
    ignoreWhitespacesAndComments();
    return input->eof();
}

int Scanner::getLineNumber() const {
    return line_number + 1;
}

int Scanner::getColumnNumber() const {
    return column_number + 1;
}

std::string Scanner::getLastRead() const {
    return last_read;
}

Scanner::saved_pos Scanner::getPosition() {
    return {line_number, column_number, input->tellg()};
}

void Scanner::setPosition(saved_pos new_position) {
    input->clear();
    input->seekg(new_position.streampos);
    line_number = new_position.line_number;
    column_number = new_position.column_number;
}
