#include "Scanner.h"

#include <limits>
#include <tuple>
#include <functional>
#include <map>

#include <boost/range/irange.hpp>

#include "utility/CompilationError.h"

Scanner::Scanner(std::unique_ptr<std::istream> input) :
    input(std::move(input)),
    line_number(0),
    column_number(0),
    last_read() {

}

Scanner::ScopedSave::ScopedSave(Scanner& state) :
    state(state),
    save(state.getPosition()) {

}

Scanner::ScopedSave::ScopedSave(Scanner& state, const saved_position& position) :
    ScopedSave(state) {

        state.setPosition(position);
}

Scanner::ScopedSave::~ScopedSave() {
    state.setPosition(save);
}

Token Scanner::getNextToken() {
    ignoreWhitespacesAndComments();
    if (endOfFile())
        throw std::istream::failure("Unexpected end of stream");
    last_read = "";
    while (input->good() and not (isValidToken(last_read) and isLongestMatch())) {
        last_read += static_cast<char>(input->get());
    }

    column_number += last_read.length();
    const auto token = tokenFromString(last_read);

    return token;
}

Token Scanner::peekNextToken() {
    const ScopedSave _{*this};
    ignoreWhitespacesAndComments();
    const auto token = endOfFile()
        ? Token::invalid
        : getNextToken();

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
                throw CompilationError("Expected identifier, got : '" + getLastRead() + "'");
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
    while (input->good() and (isspace(input->peek()) or input->peek() == '#')) {
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
    return input->peek() == std::istream::traits_type::eof();
}

int Scanner::getLineNumber() const {
    return line_number + 1;
}

int Scanner::getColumnNumber() const {
    return column_number + 1;
}

std::string Scanner::getLine(int number) {
    const ScopedSave _{*this};
    goToBeginning();
    for (const auto line_number : boost::irange(0, number-1<0?0:number-1)) {
        std::ignore = line_number;
        input->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::string line{};
    std::getline(*input, line);
    return line;
}

void Scanner::goToBeginning() {
    input->clear();
    input->seekg(0, std::ios::beg);
}

std::string Scanner::getLastRead() const {
    return last_read;
}

Scanner::saved_position Scanner::getPosition() {
    return {line_number, column_number, input->tellg(), last_read};
}

void Scanner::setPosition(saved_position new_position) {
    input->clear();
    input->seekg(new_position.streampos);
    line_number = new_position.line_number;
    column_number = new_position.column_number;
    last_read = new_position.last_read;
}
