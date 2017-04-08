#include <boost/test/unit_test.hpp>

#include <sstream>

#include "Data.h"
#include "utility.h"

BOOST_AUTO_TEST_SUITE(BinaryReaderTests);

BOOST_AUTO_TEST_CASE(climbUpTest1) {
    Type root{"root"};
    root.types.push_back({"child", "", &root});
    BOOST_CHECK_EQUAL(&root, root.types.back().climbUp<typename_equality>("root"));
}

BOOST_AUTO_TEST_CASE(climbUpTest2) {
    Type root;
    root.types.push_back({"child1", "", &root});
    root.types.push_back({"child2", "", &root});
    BOOST_CHECK_EQUAL(&root.types.front(), root.types.back().climbUp<typename_equality>("child1"));
}

BOOST_AUTO_TEST_CASE(climbUpTest3) {
    Type root;
    root.types.push_back({"child", "", &root});
    BOOST_CHECK_EQUAL(static_cast<void*>(nullptr), root.types.back().climbUp<typename_equality>("not_existing"));
}

BOOST_AUTO_TEST_CASE(climbUpTest4) {
    Type root{"root"};
    root.types.push_back({"child1", "", &root});
    root.types.back().types.push_back({"child2", "", &root.types.back()});
    BOOST_CHECK_EQUAL(&root, root.types.back().types.back().climbUp<typename_equality>("root"));
}

BOOST_AUTO_TEST_CASE(climbDownTest1) {
    Type root{"root"};
    root.types.push_back({"child", "", &root});
    const std::vector<std::string> names{"child"};
    BOOST_CHECK_EQUAL(&root.types.back(), root.climbDown<typename_equality>(names.begin(), names.end()));
}

BOOST_AUTO_TEST_CASE(climbDownTest2) {
    Type root{"root"};
    root.types.push_back({"child1", "", &root});
    root.types.back().types.push_back({"child2", "", &root.types.back()});
    const std::vector<std::string> names{"child1", "child2"};
    BOOST_CHECK_EQUAL(&root.types.back().types.back(), root.climbDown<typename_equality>(names.begin(), names.end()));
}

BOOST_AUTO_TEST_CASE(climbDownTest3) {
    Type root{"root"};
    root.types.push_back({"child1", "", &root});
    const std::vector<std::string> names{"child2"};
    BOOST_CHECK_EQUAL(static_cast<void*>(nullptr), root.climbDown<typename_equality>(names.begin(), names.end()));
}

BOOST_AUTO_TEST_CASE(findTypeByNameTest1) {
    Type root{"root"};
    root.types.push_back({"child", "", &root});
    BOOST_CHECK_EQUAL(&root.types.back(), root.findTypeByName({"child"}));
}

BOOST_AUTO_TEST_CASE(findTypeByNameTest2) {
    Type root{"root"};
    root.types.push_back({"child", "", &root});
    BOOST_CHECK_EQUAL(&root, root.types.back().findTypeByName({"root"}));
}

BOOST_AUTO_TEST_CASE(findTypeByNameTest3) {
    Type root{"root"};
    root.types.push_back({"child", "", &root});
    BOOST_CHECK_EQUAL(&root.types.back(), root.types.back().findTypeByName({"root", "child"}));
}

BOOST_AUTO_TEST_CASE(findTypeByNameTest4) {
    Type root{"root"};
    root.types.push_back({"child", "", &root});
    BOOST_CHECK_EQUAL(static_cast<void*>(nullptr), root.types.back().findTypeByName({"root", "child", "not_exisiting"}));
}

BOOST_AUTO_TEST_CASE(findTypeByNameTest5) {
    Type root{"root"};
    root.types.push_back({"child1", "", &root});
    root.types.back().types.push_back({"child11", "", &root.types.back()});
    root.types.push_back({"child2", "", &root});

    BOOST_CHECK_EQUAL(&root.types.back(), root.types.front().types.front().findTypeByName({"root", "child2"}));
    BOOST_CHECK_EQUAL(&root.types.front().types.front(), root.types.back().findTypeByName({"root", "child1", "child11"}));
    BOOST_CHECK_EQUAL(static_cast<void*>(nullptr), root.types.back().findTypeByName({"child11"}));
}

BOOST_AUTO_TEST_CASE(nextTest1) {
    Type root{"root"};
    root.types.push_back({"child", "b", &root});
    root.types.push_back({"child", "a", &root});
    BOOST_CHECK_EQUAL(&root.types.back(), root.types.front().next(1));
}

BOOST_AUTO_TEST_CASE(nextTest2) {
    Type root{"root"};
    root.types.push_back({"child", "", &root});
    root.types.push_back({"child2", "", &root});
    BOOST_CHECK_EQUAL(static_cast<void*>(nullptr), root.types.front().next(2));
}

BOOST_AUTO_TEST_CASE(nextTest3) {
    Type root{"root"};
    root.types.push_back({"child", "", &root});
    root.types.push_back({"child2", "", &root});
    BOOST_CHECK_EQUAL(static_cast<void*>(nullptr), root.types.front().next(1));
}

BOOST_AUTO_TEST_SUITE_END();
