#include <boost/test/unit_test.hpp>

#include <sstream>

#include "utility.h"

BOOST_AUTO_TEST_SUITE(UtilityTests);

BOOST_AUTO_TEST_CASE(isValidIdentifierStartTest1) {
    BOOST_CHECK(isValidIdentifierStart('_'));
}

BOOST_AUTO_TEST_CASE(isValidIdentifierStartTest2) {
    BOOST_CHECK(isValidIdentifierStart("Kappa"[0]));
}

BOOST_AUTO_TEST_CASE(isNotValidIdentifierStartTest1) {
    BOOST_CHECK(not isValidIdentifierStart('3'));
}

BOOST_AUTO_TEST_CASE(isValidIdentifierContinuationTest1) {
    BOOST_CHECK(isValidIdentifierContinuation('_'));
}

BOOST_AUTO_TEST_CASE(isValidIdentifierContinuationTest2) {
    BOOST_CHECK(isValidIdentifierContinuation("Kappa"[2]));
}

BOOST_AUTO_TEST_CASE(isValidIdentifierContinuationTest3) {
    BOOST_CHECK(isValidIdentifierContinuation('3'));
}

BOOST_AUTO_TEST_CASE(isNotValidIdentifierContinuationTest1) {
    BOOST_CHECK(not isValidIdentifierContinuation('?'));
}

BOOST_AUTO_TEST_CASE(isValidIdentifierTest1) {
    BOOST_CHECK(isValidIdentifier("Kappa"));
}

BOOST_AUTO_TEST_CASE(isValidIdentifierTest2) {
    BOOST_CHECK(isValidIdentifier("Kappa12asd"));
}

BOOST_AUTO_TEST_CASE(isValidIdentifierTest3) {
    BOOST_CHECK(isValidIdentifier("Kappa1_2asd"));
}

BOOST_AUTO_TEST_CASE(isNotValidIdentifierTest1) {
    BOOST_CHECK(not isValidIdentifier("1Kappa"));
}

BOOST_AUTO_TEST_CASE(isNotValidIdentifierTest2) {
    BOOST_CHECK(not isValidIdentifier(""));
}

BOOST_AUTO_TEST_CASE(isNumericConstantTest1) {
    BOOST_CHECK(isNumericConstant("0123"));
}

BOOST_AUTO_TEST_CASE(isNotNumericConstantTest1) {
    BOOST_CHECK(not isNumericConstant("01a23"));
}

BOOST_AUTO_TEST_CASE(isNotNumericConstantTest2) {
    BOOST_CHECK(not isNumericConstant(""));
}

BOOST_AUTO_TEST_SUITE_END();
