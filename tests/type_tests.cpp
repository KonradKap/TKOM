#include <boost/test/unit_test.hpp>

#include <sstream>

#include "tests_utility.h"
#include "parser/Struct.h"
#include "parser/Integer.h"
#include "filler/BinaryReader.h"
#include "filler/DataArray.h"

BOOST_AUTO_TEST_SUITE(BinaryReaderTests);

BOOST_AUTO_TEST_CASE(climbUpTest1) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child = std::make_shared<Struct>("child", root);
    root->subtypes.push_back(child);
    BOOST_CHECK_EQUAL(root, child->climbUp("root"));
}

BOOST_AUTO_TEST_CASE(climbUpTest2) {
    auto root = std::make_shared<Struct>();
    auto child1 = std::make_shared<Struct>("child1", root);
    auto child2 = std::make_shared<Struct>("child2", root);
    root->subtypes.push_back(child1);
    root->subtypes.push_back(child2);
    BOOST_CHECK_EQUAL(root->subtypes.front(), child2->climbUp("child1"));
}

BOOST_AUTO_TEST_CASE(climbUpTest3) {
    auto root = std::make_shared<Struct>();
    auto child1 = std::make_shared<Struct>("child", root);
    root->subtypes.push_back(child1);
    BOOST_CHECK_EQUAL(std::shared_ptr<Struct>(nullptr), child1->climbUp("not_existing"));
}

BOOST_AUTO_TEST_CASE(climbUpTest4) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child1 = std::make_shared<Struct>("child1", root);
    auto child2 = std::make_shared<Struct>("child2", child1);
    root->subtypes.push_back(child1);
    child1->subtypes.push_back(child2);
    BOOST_CHECK_EQUAL(root, child2->climbUp("root"));
}

BOOST_AUTO_TEST_CASE(climbDownTest1) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child = std::make_shared<Struct>("child", root);
    root->subtypes.push_back(child);
    const std::vector<std::string> names{"child"};
    BOOST_CHECK_EQUAL(root->subtypes.back(), root->climbDown(names.begin(), names.end()));
}

BOOST_AUTO_TEST_CASE(climbDownTest3) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child = std::make_shared<Struct>("child1", root);
    root->subtypes.push_back(child);
    const std::vector<std::string> names{"child2"};
    BOOST_CHECK_EQUAL(std::shared_ptr<Struct>(nullptr), root->climbDown(names.begin(), names.end()));
}

BOOST_AUTO_TEST_CASE(findSubtypeTest1) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child = std::make_shared<Struct>("child", root);
    root->subtypes.push_back(child);
    BOOST_CHECK_EQUAL(root->subtypes.back(), root->Type::findSubtype(std::vector<std::string>{"child"}));
}

BOOST_AUTO_TEST_CASE(findSubtypeTest2) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child = std::make_shared<Struct>("child", root);
    root->subtypes.push_back(child);
    BOOST_CHECK_EQUAL(root, child->Type::findSubtype({"root"}));
}

BOOST_AUTO_TEST_CASE(findSubtypeTest3) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child = std::make_shared<Struct>("child", root);
    root->subtypes.push_back(child);
    BOOST_CHECK_EQUAL(root->subtypes.back(), child->Type::findSubtype({"root", "child"}));
}

BOOST_AUTO_TEST_CASE(findSubtypeTest4) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child = std::make_shared<Struct>("child", root);
    root->subtypes.push_back(child);
    BOOST_CHECK_EQUAL(std::shared_ptr<Struct>(nullptr), child->Type::findSubtype({"root", "child", "not_exisiting"}));
}

BOOST_AUTO_TEST_CASE(findSubtypeTest5) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child1 = std::make_shared<Struct>("child1", root);
    root->subtypes.push_back(child1);

    auto child11 = std::make_shared<Struct>("child11", child1);
    child1->subtypes.push_back(child11);

    auto child2 = std::make_shared<Struct>("child2", root);
    root->subtypes.push_back(child2);

    BOOST_CHECK_EQUAL(root->subtypes.back(), child11->Type::findSubtype({"root", "child2"}));
    BOOST_CHECK_EQUAL(std::shared_ptr<Struct>(nullptr), child2->Type::findSubtype({"child11"}));
}

BOOST_AUTO_TEST_CASE(wasDeclaredTest1) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child1 = std::make_shared<Struct>("root", root);
    root->subtypes.push_back(child1);

    auto child2 = std::make_shared<Struct>("root", child1);
    child1->subtypes.push_back(child2);

    BOOST_CHECK(not child2->wasDeclared("root"));
    BOOST_CHECK(child1->wasDeclared("root"));
    BOOST_CHECK(root->wasDeclared("root"));
}

BOOST_AUTO_TEST_CASE(wasDeclaredTest2) {
    auto root = std::make_shared<Struct>("root", std::weak_ptr<Struct>());
    auto child1 = std::make_shared<Struct>("root", root);
    root->subtypes.push_back(child1);

    BOOST_CHECK(not child1->wasDeclared("root"));
    BOOST_CHECK(root->wasDeclared("root"));
}

BOOST_AUTO_TEST_CASE(applyTest1) {
    auto int_ = std::make_shared<Integer>(std::weak_ptr<Type>());
    std::unique_ptr<std::stringstream> ss =
        std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    *ss << "\x01";
    BinaryReader reader{std::move(ss)};
    DataArray array{};
    array.applied_arguments.insert({{}, 8});

    int_->apply(reader, array);
    BOOST_CHECK(array.has_value);
    BOOST_CHECK_EQUAL(1, array.value);
}

BOOST_AUTO_TEST_CASE(applyTest2) {
    auto struct_ = std::make_shared<Struct>("root", std::weak_ptr<Type>());
    auto int_ = std::make_shared<Integer>(std::weak_ptr<Type>());
    struct_->declared.push_back({int_, {}, makeShallowList({"1"}), {makeShallowList({"8"})}});
    std::unique_ptr<std::stringstream> ss =
        std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    *ss << "\x01\x02\x03\x04";
    BinaryReader reader{std::move(ss)};
    DataArray array{};

    struct_->apply(reader, array);
    BOOST_CHECK(array.children.front().has_value);
    BOOST_CHECK_EQUAL(1, array.children.front().value);
}

BOOST_AUTO_TEST_CASE(applyTest3) {
    auto struct_ = std::make_shared<Struct>("root", std::weak_ptr<Type>());
    auto int_ = std::make_shared<Integer>(std::weak_ptr<Type>());
    struct_->declared.push_back({int_, {}, makeShallowList({"1"}), {makeShallowList({"8"})}});
    struct_->declared.push_back({int_, {}, makeShallowList({"1"}), {makeShallowList({"4"})}});
    std::unique_ptr<std::stringstream> ss =
        std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    *ss << "\x01\x30\x03\x04";
    BinaryReader reader{std::move(ss)};
    DataArray array{};

    struct_->apply(reader, array);
    BOOST_CHECK(array.children.front().has_value);
    BOOST_CHECK_EQUAL(1, array.children.front().value);
    BOOST_CHECK(array.children.back().has_value);
    BOOST_CHECK_EQUAL(3, array.children.back().value);
}

BOOST_AUTO_TEST_SUITE_END();
