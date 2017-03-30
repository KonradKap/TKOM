#include <boost/test/unit_test.hpp>

#include <sstream>

#include "Scanner.h"

BOOST_AUTO_TEST_SUITE(TokenTests)

BOOST_AUTO_TEST_CASE(tokenFromStringStruct) {
    const auto token = tokenFromString("struct");
    BOOST_CHECK_EQUAL(Token::struct_keyword, token);
}

BOOST_AUTO_TEST_CASE(tokenFromStringIdentifier) {
    const auto token = tokenFromString("asdf");
    BOOST_CHECK_EQUAL(Token::identifier, token);
}

BOOST_AUTO_TEST_CASE(tokenFromStringIntConst) {
    const auto token = tokenFromString("1234");
    BOOST_CHECK_EQUAL(Token::integer_value, token);
}

BOOST_AUTO_TEST_CASE(tokenFromStringInvalid) {
    const auto token = tokenFromString("123a4");
    BOOST_CHECK_EQUAL(Token::invalid, token);
}

BOOST_AUTO_TEST_CASE(tokenFromStringWithWhitespacesIsInvalid) {
    const auto token = tokenFromString("1234 ");
    BOOST_CHECK_EQUAL(Token::invalid, token);
}

BOOST_AUTO_TEST_CASE(toStringUnionId) {
    const auto token = toString(Token::union_keyword);
    BOOST_CHECK_EQUAL("union", token);
}

BOOST_AUTO_TEST_CASE(toStringIdentifier) {
    const auto token = toString(Token::identifier);
    BOOST_CHECK_EQUAL("identifier", token);
}

BOOST_AUTO_TEST_SUITE_END()
