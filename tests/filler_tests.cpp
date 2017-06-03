#include <boost/test/unit_test.hpp>

#include "filler/Filler.h"
#include "parser/Integer.h"
#include "parser/Struct.h"
#include "tests_utility.h"

BOOST_AUTO_TEST_SUITE(ParserTests);

BOOST_AUTO_TEST_CASE(FillTest1) {
//    auto root = std::make_shared<Struct>("", std::weak_ptr<Type>());
//    auto integer = std::make_shared<Integer>(root);
//    root->declared.push_back({integer, "a", makeShallowList({"1"}), {makeShallowList({"32"})}});
//    auto reader = readerFromString("\x0001");
//    Filler filler{reader, root};
//    Data expected{"a", {}, {}, {DataChild{{}, 1}}};
//    BOOST_CHECK_EQUAL(expected, filler.fill());
}

BOOST_AUTO_TEST_CASE(ResolveArgumentTest1) {
    //auto root = std::make_shared<Struct>("", std::weak_ptr<Type>());
    //auto reader = readerFromString("\x0001");
    //Filler filler{reader, root};
    DataArray array{};
    BOOST_CHECK_EQUAL(3, array.resolveArgument(makeShallowList({"3"})));
}

BOOST_AUTO_TEST_SUITE_END();
