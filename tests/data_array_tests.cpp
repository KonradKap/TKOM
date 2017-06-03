#include <boost/test/unit_test.hpp>

#include "filler/DataArray.h"
#include "utility/CompilationError.h"

#include "tests_utility.h"

BOOST_AUTO_TEST_SUITE(DataArrayTests);

BOOST_AUTO_TEST_CASE(ResolveArgumentTests1) {
    const auto data = makeDataArray("", 0);
    BOOST_CHECK_EQUAL(0, data.resolveArgument(makeShallowList({})));
}

BOOST_AUTO_TEST_CASE(ResolveArgumentTests2) {
    auto data = makeDataArray("a");
    auto child = makeDataArray("b", 10);
    child.parent = &data;
    data.children.push_back(child);
    BOOST_CHECK_EQUAL(10, data.resolveArgument(makeShallowList({"b"})));
    BOOST_CHECK_THROW(data.resolveArgument(makeShallowList({"a"})), CompilationError);
}

BOOST_AUTO_TEST_CASE(ResolveArgumentTests3) {
    auto data = makeDataArray("a");
    auto child1 = makeDataArray("b");
    child1.parent = &data;
    auto child2 = makeDataArray("c", 20);
    child2.parent = &child1;
    child1.children.push_back(child2);
    data.children.push_back(child1);

    BOOST_CHECK_THROW(data.resolveArgument(makeShallowList({"a", "b"})), CompilationError);
    BOOST_CHECK_EQUAL(20, data.resolveArgument(makeShallowList({"b", "c"})));
    BOOST_CHECK_EQUAL(20, child1.resolveArgument(makeShallowList({"c"})));
}

BOOST_AUTO_TEST_CASE(ResolveArgumentTests4) {
    auto root = makeDataArray("");
    auto data = makeDataArray("a");
    auto child1 = makeDataArray("b", 10);
    child1.parent = &data;
    data.children.push_back(child1);
    auto child2 = makeDataArray("c", 20);
    child2.parent = &data;
    data.children.push_back(child2);
    data.parent = &root;
    root.children.push_back(data);
    BOOST_CHECK_EQUAL(10, data.resolveArgument(makeShallowList({"b"})));
    BOOST_CHECK_EQUAL(20, data.resolveArgument(makeShallowList({"c"})));
    BOOST_CHECK_EQUAL(20, child1.resolveArgument(makeShallowList({"a", "c"})));
    BOOST_CHECK_THROW(child2.resolveArgument(makeShallowList({"d"})), CompilationError);
}

BOOST_AUTO_TEST_CASE(ResolveArgumentTests5) {
    auto data = makeDataArray("a");
    auto child1 = makeDataArray("a");
    child1.parent = &data;
    auto child2 = makeDataArray("a", 20);
    child2.parent = &child1;
    child1.children.push_back(child2);
    data.children.push_back(child1);

    BOOST_CHECK_THROW(data.resolveArgument(makeShallowList({"a", "a", "a", "a", "a", "a"})), CompilationError);
    BOOST_CHECK_THROW(data.resolveArgument(makeShallowList({"a", "a", "a", "a", "a", "a", "a"})), CompilationError);
}

BOOST_AUTO_TEST_CASE(ClimbUpTest1) {
    auto data = makeDataArray("a");
    auto child1 = makeDataArray("a");
    child1.parent = &data;
    BOOST_CHECK_EQUAL(&data, child1.climbUp("a"));
}

BOOST_AUTO_TEST_CASE(ClimbUpTest2) {
    auto data = makeDataArray("a");
    auto child1 = makeDataArray("b");
    child1.parent = &data;
    auto child2 = makeDataArray("c", 20);
    child2.parent = &child1;
    child1.children.push_back(child2);
    data.children.push_back(child1);

    BOOST_CHECK_EQUAL(&data, child2.climbUp("a"));
}

BOOST_AUTO_TEST_CASE(ClimbUpTest3) {
    auto data = makeDataArray("a");
    auto child1 = makeDataArray("a");
    child1.parent = &data;
    data.children.push_back(child1);
    BOOST_CHECK_EQUAL(static_cast<void*>(nullptr), child1.climbUp("not exisiting"));
}

BOOST_AUTO_TEST_CASE(NearestChildTest1) {
    auto data = makeDataArray("a");
    auto child1 = makeDataArray("a");
    child1.parent = &data;
    data.children.push_back(child1);
    BOOST_CHECK_EQUAL(data.children.front(), *data.nearestChild("a"));
}

BOOST_AUTO_TEST_CASE(NearestChildTest2) {
    auto data = makeDataArray("a");
    auto child1 = makeDataArray("a");
    child1.parent = &data;
    auto child2 = makeDataArray("b");
    child1.parent = &data;
    data.children.push_back(child1);
    data.children.push_back(child2);
    BOOST_CHECK_EQUAL(data.children.back(), *data.nearestChild("b"));
}

BOOST_AUTO_TEST_CASE(GetIndexTest1) {
    auto data = makeDataArray("a");
    ReferenceList::Element root{};
    root.list.elements.push_back(makeShallowList({"3"}).elements.front());
    BOOST_CHECK_EQUAL(3, data.getIndex(root));
}

BOOST_AUTO_TEST_SUITE_END();
