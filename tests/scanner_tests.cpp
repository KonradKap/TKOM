#include <boost/test/unit_test.hpp>

#include <sstream>
#include <istream>
#include <tuple>

#include "Scanner.h"
#include "Token.h"
#include "print_log_value.h"

BOOST_AUTO_TEST_SUITE(ScannerTests);

BOOST_AUTO_TEST_CASE(ScanEmptyInput) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "";
    Scanner scanner(std::move(ss));
    BOOST_CHECK_THROW(scanner.getNextToken(), std::istream::failure);
}

BOOST_AUTO_TEST_CASE(ScanStruct) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "struct";
    Scanner scanner(std::move(ss));
    BOOST_CHECK_EQUAL(Token::struct_keyword, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(ScanWithWhitespaces) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "   case";
    Scanner scanner(std::move(ss));
    BOOST_CHECK_EQUAL(Token::case_keyword, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(ScanTwice) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "   (  \n  kappa   ";
    Scanner scanner(std::move(ss));
    std::ignore = scanner.getNextToken();
    BOOST_CHECK_EQUAL(Token::identifier, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(ScanTokensNotSeparatedByWhitespace) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "int(3)";
    Scanner scanner(std::move(ss));
    std::ignore = scanner.getNextToken();
    BOOST_CHECK_EQUAL(Token::round_bracket_begin, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(EnsureLongestTokenRuleTest1) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "asdf";
    Scanner scanner(std::move(ss));
    BOOST_CHECK_EQUAL(Token::identifier, scanner.getNextToken());
    BOOST_CHECK_EQUAL(4, scanner.getLastRead().length());
}

BOOST_AUTO_TEST_CASE(EnsureLongestTokenRuleTest2) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "asdf1234";
    Scanner scanner(std::move(ss));
    BOOST_CHECK_EQUAL(Token::identifier, scanner.getNextToken());
    BOOST_CHECK_EQUAL(8, scanner.getLastRead().length());
}

BOOST_AUTO_TEST_CASE(getLineAndColumnNumbersTest1) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "asdfa  ";
    Scanner scanner(std::move(ss));
    std::ignore = scanner.getNextToken();
    BOOST_CHECK_EQUAL(6, scanner.getColumnNumber());
    BOOST_CHECK_EQUAL(1, scanner.getLineNumber());
}

BOOST_AUTO_TEST_CASE(getLineAndColumnNumbersTest2) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "\n\n; asdf  ";
    Scanner scanner(std::move(ss));
    std::ignore = scanner.getNextToken();
    BOOST_CHECK_EQUAL(2, scanner.getColumnNumber());
    BOOST_CHECK_EQUAL(3, scanner.getLineNumber());
}

BOOST_AUTO_TEST_CASE(PeekNextTokenTest1) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "int asdf";
    Scanner scanner(std::move(ss));
    BOOST_CHECK_EQUAL(Token::int_keyword, scanner.peekNextToken());
}

BOOST_AUTO_TEST_CASE(PeekNextTokenTest2) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "int asdf";
    Scanner scanner(std::move(ss));
    scanner.peekNextToken();
    BOOST_CHECK_EQUAL("", scanner.getLastRead());
    BOOST_CHECK_EQUAL(1, scanner.getLineNumber());
    BOOST_CHECK_EQUAL(1, scanner.getColumnNumber());
}

BOOST_AUTO_TEST_CASE(IgnoreComments) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "int #asdf\ncase";
    Scanner scanner(std::move(ss));
    std::ignore = scanner.getNextToken();
    BOOST_CHECK_EQUAL(Token::case_keyword, scanner.getNextToken());
    BOOST_CHECK_EQUAL(2, scanner.getLineNumber());
    BOOST_CHECK_EQUAL(5, scanner.getColumnNumber());
}

BOOST_AUTO_TEST_CASE(ReadUntill) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "int #asdf\n; asd case asdf";
    Scanner scanner(std::move(ss));
    scanner.readUntill(Token::case_keyword);
    BOOST_CHECK_EQUAL(Token::case_keyword, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(readDeepIdentifierTest1) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "asdf.fdsa.asdf";
    Scanner scanner(std::move(ss));
    BOOST_CHECK_EQUAL(std::vector<std::string>({"asdf", "fdsa", "asdf"}), scanner.readDeepIdentifier());
}

BOOST_AUTO_TEST_CASE(readDeepIdentifierTest2) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << "asdf[";
    Scanner scanner(std::move(ss));
    BOOST_CHECK_EQUAL(std::vector<std::string>({"asdf"}), scanner.readDeepIdentifier());
}

BOOST_AUTO_TEST_SUITE_END();
