#include <boost/test/unit_test.hpp>

#include "parser/ReferenceList.h"

BOOST_AUTO_TEST_SUITE(ReferenceListTests);

BOOST_AUTO_TEST_CASE(isEmptyTest1) {
    ReferenceList rl;
    BOOST_CHECK(rl.empty());
}

BOOST_AUTO_TEST_CASE(isEmptyTest2) {
    ReferenceList rl;
    rl.addElement({{}, "2"});
    BOOST_CHECK(not rl.empty());
}

BOOST_AUTO_TEST_SUITE_END();
