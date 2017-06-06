#include <boost/test/unit_test.hpp>

#include "lexer/Scanner.h"
#include "lexer/Token.h"
#include "utility/CompilationError.h"
#include "print_log_value.h"
#include "tests_utility.h"

BOOST_AUTO_TEST_SUITE(ScannerTests);

BOOST_AUTO_TEST_CASE(ScanEmptyInput) {
    Scanner scanner = scannerFromString("");
    BOOST_CHECK_THROW(scanner.getNextToken(), std::istream::failure);
}

BOOST_AUTO_TEST_CASE(ScanStruct) {
    Scanner scanner = scannerFromString("struct");
    BOOST_CHECK_EQUAL(Token::struct_keyword, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(ScanWithWhitespaces) {
    Scanner scanner = scannerFromString("   case");
    BOOST_CHECK_EQUAL(Token::case_keyword, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(ScanTwice) {
    Scanner scanner = scannerFromString("   (  \n  kappa   ");
    std::ignore = scanner.getNextToken();
    BOOST_CHECK_EQUAL(Token::identifier, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(ScanTokensNotSeparatedByWhitespace) {
    Scanner scanner = scannerFromString("int(3)");
    BOOST_CHECK_EQUAL(Token::int_keyword, scanner.getNextToken());
    BOOST_CHECK_EQUAL(Token::round_bracket_begin, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(EnsureLongestTokenRuleTest1) {
    Scanner scanner = scannerFromString("asdf");
    BOOST_CHECK_EQUAL(Token::identifier, scanner.getNextToken());
    BOOST_CHECK_EQUAL(4, scanner.getLastRead().length());
}

BOOST_AUTO_TEST_CASE(EnsureLongestTokenRuleTest2) {
    Scanner scanner = scannerFromString("asdf1234");
    BOOST_CHECK_EQUAL(Token::identifier, scanner.getNextToken());
    BOOST_CHECK_EQUAL(8, scanner.getLastRead().length());
}

BOOST_AUTO_TEST_CASE(EnsureLongestTokenRuleTest3) {
    Scanner scanner = scannerFromString("1234asdf");
    BOOST_CHECK_THROW(scanner.getNextToken(), CompilationError);
    BOOST_CHECK_EQUAL(5, scanner.getLastRead().length());
}

BOOST_AUTO_TEST_CASE(getLineAndColumnNumbersTest1) {
    Scanner scanner = scannerFromString("asdfa   ");
    BOOST_CHECK_EQUAL(Token::identifier, scanner.getNextToken());
    BOOST_CHECK_EQUAL(6, scanner.getColumnNumber());
    BOOST_CHECK_EQUAL(1, scanner.getLineNumber());
}

BOOST_AUTO_TEST_CASE(getLineAndColumnNumbersTest2) {
    Scanner scanner = scannerFromString("\n\n; asdf  ");
    std::ignore = scanner.getNextToken();
    BOOST_CHECK_EQUAL(2, scanner.getColumnNumber());
    BOOST_CHECK_EQUAL(3, scanner.getLineNumber());
}

BOOST_AUTO_TEST_CASE(PeekNextTokenTest1) {
    Scanner scanner = scannerFromString("int asdf");
    BOOST_CHECK_EQUAL(Token::int_keyword, scanner.peekNextToken());
}

BOOST_AUTO_TEST_CASE(PeekNextTokenTest2) {
    Scanner scanner = scannerFromString("int asdf");
    scanner.peekNextToken();
    BOOST_CHECK_EQUAL("", scanner.getLastRead());
    BOOST_CHECK_EQUAL(1, scanner.getLineNumber());
    BOOST_CHECK_EQUAL(1, scanner.getColumnNumber());
}

BOOST_AUTO_TEST_CASE(IgnoreComments) {
    Scanner scanner = scannerFromString("int #asdf\ncase");
    std::ignore = scanner.getNextToken();
    BOOST_CHECK_EQUAL(Token::case_keyword, scanner.getNextToken());
    BOOST_CHECK_EQUAL(2, scanner.getLineNumber());
    BOOST_CHECK_EQUAL(5, scanner.getColumnNumber());
}

BOOST_AUTO_TEST_CASE(ReadUntill) {
    Scanner scanner = scannerFromString("int #asdf\n; asd case asdf");
    scanner.readUntill(Token::case_keyword);
    BOOST_CHECK_EQUAL(Token::case_keyword, scanner.getNextToken());
}

BOOST_AUTO_TEST_CASE(readDeepIdentifierTest1) {
    Scanner scanner = scannerFromString("asdf.fdsa.asdf");
    BOOST_CHECK_EQUAL(std::vector<std::string>({"asdf", "fdsa", "asdf"}), scanner.readDeepIdentifier());
}

BOOST_AUTO_TEST_CASE(readDeepIdentifierTest2) {
    Scanner scanner = scannerFromString("asdf[");
    BOOST_CHECK_EQUAL(std::vector<std::string>({"asdf"}), scanner.readDeepIdentifier());
}

BOOST_AUTO_TEST_SUITE_END();
