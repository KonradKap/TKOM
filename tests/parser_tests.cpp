#include <boost/test/unit_test.hpp>

#include "parser/Parser.h"
#include "utility/CompilationError.h"
#include "parser/Struct.h"
#include "lexer/Scanner.h"
#include "tests_utility.h"

#include "print_log_value.h"

BOOST_AUTO_TEST_SUITE(ParserTests);

BOOST_AUTO_TEST_CASE(ParseReferenceListTest1) {
    auto scanner = scannerFromString("[a.b.c.d]");
    Parser parser{scanner};
    const auto expected = makeShallowList({"a", "b", "c", "d"});
    BOOST_CHECK_EQUAL(expected, parser.parseReferenceList(squareBrackets()));
}

BOOST_AUTO_TEST_CASE(ParseReferenceListTest2) {
    auto scanner = scannerFromString("(a.b[c].d) g");
    Parser parser{scanner};
    const auto b_sublist = makeShallowList({"c"});
    const ReferenceList expected = ReferenceList{}
        .addElement({{}, "a"})
        .addElement({b_sublist, "b"})
        .addElement({{}, "d"});
    BOOST_CHECK_EQUAL(expected, parser.parseReferenceList(roundBrackets()));
}

BOOST_AUTO_TEST_CASE(ParseReferenceListTest3) {
    auto scanner = scannerFromString("(a.b[c]) g");
    Parser parser{scanner};
    const auto b_sublist = makeShallowList({"c"});
    const ReferenceList expected = ReferenceList{}
        .addElement({{}, "a"})
        .addElement({b_sublist, "b"});
    BOOST_CHECK_EQUAL(expected, parser.parseReferenceList(roundBrackets()));
}

BOOST_AUTO_TEST_CASE(ParseReferenceListTest4) {
    auto scanner = scannerFromString("[a.b[c.d[e]].d] struct");
    Parser parser{scanner};
    const auto d_sublist = makeShallowList({"e"});
    const ReferenceList b_sublist = ReferenceList{}
        .addElement({{}, "c"})
        .addElement({d_sublist, "d"});
    const ReferenceList expected = ReferenceList{}
        .addElement({{}, "a"})
        .addElement({b_sublist, "b"})
        .addElement({{}, "d"});

    BOOST_CHECK_EQUAL(expected, parser.parseReferenceList(squareBrackets()));
}

BOOST_AUTO_TEST_CASE(ParseReferenceListTest5) {
    auto scanner = scannerFromString("(a.b[123]) 123");
    Parser parser{scanner};
    const auto b_sublist = makeShallowList({"123"});
    const ReferenceList expected = ReferenceList{}
        .addElement({{}, "a"})
        .addElement({b_sublist, "b"});
    BOOST_CHECK_EQUAL(expected, parser.parseReferenceList(roundBrackets()));
}

BOOST_AUTO_TEST_CASE(ParseReferenceListTest6) {
    auto scanner = scannerFromString("{a.b[123].123 123}");
    Parser parser{scanner};
    BOOST_CHECK_THROW(parser.parseReferenceList(curlyBrackets()), CompilationError);
}

BOOST_AUTO_TEST_CASE(ParseReferenceListTest7) {
    auto scanner = scannerFromString("(a.struct[a].a) 123");
    Parser parser{scanner};
    BOOST_CHECK_THROW(parser.parseReferenceList(roundBrackets()), CompilationError);
}

BOOST_AUTO_TEST_CASE(ParseReferenceListTest8) {
    auto scanner = scannerFromString("[   123   ]");
    Parser parser{scanner};
    const auto expected = makeShallowList({"123"});
    BOOST_CHECK_EQUAL(expected, parser.parseReferenceList(squareBrackets()));
}

BOOST_AUTO_TEST_CASE(ParseReferenceListTest9) {
    auto scanner = scannerFromString("[]");
    Parser parser{scanner};
    const auto expected = makeShallowList({});
    BOOST_CHECK_EQUAL(expected, parser.parseReferenceList(squareBrackets()));
}

BOOST_AUTO_TEST_CASE(ParseReferenceListTest10) {
    auto scanner = scannerFromString("a.b  ;");
    Parser parser{scanner};
    const auto expected = makeShallowList({"a", "b"});
    BOOST_CHECK_EQUAL(expected, parser.parseReferenceList({Token::any, Token::any}));
    BOOST_CHECK_EQUAL(Token::semicolon, scanner.peekNextToken());
}

BOOST_AUTO_TEST_CASE(ParseStructTest1) {
    auto scanner = scannerFromString("struct() Kappa { int; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Struct>("Kappa", result);
    expected->declared.push_back({parser.Int, "", makeShallowList({"1"}), {makeShallowList({"32"})}});
    parser.parseStructDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseArgumentsListTest1) {
    auto scanner = scannerFromString("(a, b, c, d)");
    Parser parser{scanner};
    const std::vector<std::string> expected{"a", "b", "c", "d"};
    BOOST_CHECK_EQUAL(expected, parser.parseArgumentsList());
}

BOOST_AUTO_TEST_CASE(ParseArgumentsListTest2) {
    auto scanner = scannerFromString("()");
    Parser parser{scanner};
    const std::vector<std::string> expected{};
    BOOST_CHECK_EQUAL(expected, parser.parseArgumentsList());
}

BOOST_AUTO_TEST_CASE(ParseArgumentsListTest3) {
    auto scanner = scannerFromString("(a)");
    Parser parser{scanner};
    const std::vector<std::string> expected{"a"};
    BOOST_CHECK_EQUAL(expected, parser.parseArgumentsList());
}

BOOST_AUTO_TEST_CASE(ParseArgumentsListTest4) {
    auto scanner = scannerFromString("(a, struct)");
    Parser parser{scanner};
    BOOST_CHECK_THROW(parser.parseArgumentsList(), CompilationError);
}

BOOST_AUTO_TEST_CASE(ParseArgumentsListTest5) {
    auto scanner = scannerFromString("(a, )");
    Parser parser{scanner};
    BOOST_CHECK_THROW(parser.parseArgumentsList(), CompilationError);
}

BOOST_AUTO_TEST_CASE(ParseArgumentsListTest6) {
    auto scanner = scannerFromString("(a b)");
    Parser parser{scanner};
    BOOST_CHECK_THROW(parser.parseArgumentsList(), CompilationError);
}

BOOST_AUTO_TEST_CASE(ParseArgumentsListTest7) {
    auto scanner = scannerFromString("(a % b)");
    Parser parser{scanner};
    BOOST_CHECK_THROW(parser.parseArgumentsList(), CompilationError);
}

BOOST_AUTO_TEST_CASE(ParseIntegerUsageTest1) {
    auto scanner = scannerFromString("int;");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = DeclaredData{parser.Int, "", makeShallowList({"1"}), {makeShallowList({"32"})}};
    parser.parseIntegerUsage(result);
    BOOST_CHECK_EQUAL(expected, result->declared.back());
}

BOOST_AUTO_TEST_CASE(CheckForArgumentsTest1) {
    auto scanner = scannerFromString("(a.b)");
    Parser parser{scanner};
    std::vector<ReferenceList> expected{makeShallowList({"a", "b"})};
    BOOST_CHECK_EQUAL(expected, parser.checkForArguments());
}

BOOST_AUTO_TEST_CASE(CheckForArgumentsTest2) {
    auto scanner = scannerFromString("[a.b]");
    Parser parser{scanner};
    std::vector<ReferenceList> expected{};
    BOOST_CHECK_EQUAL(expected, parser.checkForArguments());
}

BOOST_AUTO_TEST_CASE(CheckForArgumentsTest3) {
    auto scanner = scannerFromString("() as");
    Parser parser{scanner};
    std::vector<ReferenceList> expected{};
    BOOST_CHECK_EQUAL(expected, parser.checkForArguments());
    BOOST_CHECK_EQUAL(Token::identifier, scanner.peekNextToken());
}

BOOST_AUTO_TEST_CASE(CheckForArgumentsTest4) {
    auto scanner = scannerFromString("(a.b, c.d)");
    Parser parser{scanner};
    std::vector<ReferenceList> expected{makeShallowList({"a", "b"}), makeShallowList({"c", "d"})};
    BOOST_CHECK_EQUAL(expected, parser.checkForArguments());
}

BOOST_AUTO_TEST_CASE(ParseIntegerUsageTest2) {
    auto scanner = scannerFromString("int(3)[5] asdf;");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = DeclaredData{parser.Int, "asdf", makeShallowList({"5"}), {makeShallowList({"3"})}};
    parser.parseIntegerUsage(result);
    BOOST_CHECK_EQUAL(expected, result->declared.back());
}

BOOST_AUTO_TEST_CASE(ParseIntegerUsageTest3) {
    auto scanner = scannerFromString("int() asdf;");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = DeclaredData{parser.Int, "asdf", makeShallowList({"1"}), {makeShallowList({"32"})}};
    parser.parseIntegerUsage(result);
    BOOST_CHECK_EQUAL(expected, result->declared.back());
}

BOOST_AUTO_TEST_CASE(ParseIntegerUsageTest4) {
    auto scanner = scannerFromString("int(3, 3)[5] asdf;");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    BOOST_CHECK_THROW(parser.parseIntegerUsage(result), CompilationError);
}

BOOST_AUTO_TEST_CASE(ParseIntegerUsageTest5) {
    auto scanner = scannerFromString("int asdf; int asdf;");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    parser.parseIntegerUsage(result);
    BOOST_CHECK(result->wasUsed("asdf"));
    BOOST_CHECK_THROW(parser.parseIntegerUsage(result), CompilationError);
}

BOOST_AUTO_TEST_CASE(ParseStructTest2) {
    auto scanner = scannerFromString("struct() Kappa { int; int; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Struct>("Kappa", result);
    expected->declared.push_back({parser.Int, "", makeShallowList({"1"}), {makeShallowList({"32"})}});
    expected->declared.push_back({parser.Int, "", makeShallowList({"1"}), {makeShallowList({"32"})}});
    parser.parseStructDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseStructTest3) {
    auto scanner = scannerFromString("struct() Kappa { struct() Keepo { int; }; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Struct>("Kappa", result);
    auto keepo = std::make_shared<Struct>("Keepo", expected);
    keepo->declared.push_back({parser.Int, "", makeShallowList({"1"}), {makeShallowList({"32"})}});
    expected->subtypes.push_back(keepo);
    parser.parseStructDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseStructTest4) {
    auto scanner = scannerFromString("struct() Kappa { struct() Kappa { int; }; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Struct>("Kappa", result);
    auto kappa = std::make_shared<Struct>("Kappa", expected);
    kappa->declared.push_back({parser.Int, "", makeShallowList({"1"}), {makeShallowList({"32"})}});
    expected->subtypes.push_back(kappa);
    parser.parseStructDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseStructTest5) {
    auto scanner = scannerFromString("struct() Kappa { int; }; struct() Kappa { int; }; ");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    parser.parseStructDeclaration(result);
    BOOST_CHECK_THROW(parser.parseStructDeclaration(result), CompilationError);
}

BOOST_AUTO_TEST_CASE(ParseStructTest6) {
    auto scanner = scannerFromString("struct() Kappa {};");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Struct>("Kappa", result);
    parser.parseStructDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseStructTest7) {
    auto scanner = scannerFromString("struct() Kappa { struct() Kappa { struct() Kappa {}; }; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Struct>("Kappa", result);
    auto kappa = std::make_shared<Struct>("Kappa", expected);
    kappa->subtypes.push_back(std::make_shared<Struct>("Kappa", kappa));
    expected->subtypes.push_back(kappa);
    parser.parseStructDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseIdentifierUsage1) {
    auto scanner = scannerFromString("Kappa();");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    result->subtypes.push_back(std::make_shared<Struct>("Kappa", result));
    auto expected = DeclaredData{result->subtypes.back(), "", makeShallowList({"1"}), {}};
    parser.parseIdentifierUsage(result);
    BOOST_CHECK_EQUAL(expected, result->declared.back());
}

//BOOST_AUTO_TEST_CASE(ParseIdentifierUsage2) {
//    auto scanner = scannerFromString("Kappa.Kappa();");
//    Parser parser{scanner};
//    auto result = std::make_shared<Struct>();
//    result->subtypes.push_back(std::make_shared<Struct>("Kappa", result));
//    result->subtypes.back()->subtypes.push_back(std::make_shared<Struct>("Kappa", result->subtypes.back()));
//    auto expected = DeclaredData{
//        result->subtypes.back()->subtypes.back(), "", makeShallowList({"1"}), {}};
//    parser.parseIdentifierUsage(result);
//    BOOST_CHECK_EQUAL(expected, result->declared.back());
//}

//BOOST_AUTO_TEST_CASE(ParseIdentifierUsage3) {
//    auto scanner = scannerFromString("Kappa.Kapa();");
//    Parser parser{scanner};
//    auto result = std::make_shared<Struct>();
//    result->subtypes.push_back(std::make_shared<Struct>("Kappa", result));
//    result->subtypes.back()->subtypes.push_back(std::make_shared<Struct>("Kappa", result->subtypes.back()));
//    BOOST_CHECK_THROW(parser.parseIdentifierUsage(result), CompilationError);
//}

BOOST_AUTO_TEST_CASE(ParseIdentifierUsage4) {
    auto scanner = scannerFromString("Kappa(a, b, c) kapa;");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    result->subtypes.push_back(std::make_shared<Struct>("Kappa", result,
        std::vector<std::string>{"x", "y", "z"}));
    auto expected = DeclaredData{
        result->subtypes.back(), "kapa", makeShallowList({"1"}),
        {makeShallowList({"a"}), makeShallowList({"b"}), makeShallowList({"c"})}};
    parser.parseIdentifierUsage(result);
    BOOST_CHECK_EQUAL(expected, result->declared.back());
}

BOOST_AUTO_TEST_CASE(ParseUnionTest1) {
    auto scanner = scannerFromString("union Kappa { case(1) { int; }; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Union>("Kappa", result);
    auto case_1 = std::make_shared<Struct>("", expected);
    case_1->declared.push_back({parser.Int, "", makeShallowList({"1"}), {makeShallowList({"32"})}});
    expected->cases.push_back({makeShallowList({"1"}), case_1});
    parser.parseUnionDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseUnionTest2) {
    auto scanner = scannerFromString("union Kappa { };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Union>("Kappa", result);
    parser.parseUnionDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseUnionTest3) {
    auto scanner = scannerFromString("union Kappa { case(1) { int; }; case(a.b.c) { }; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Union>("Kappa", result);
    auto case_1 = std::make_shared<Struct>("", expected);
    auto case_abc = std::make_shared<Struct>("", expected);
    case_1->declared.push_back({parser.Int, "", makeShallowList({"1"}), {makeShallowList({"32"})}});
    expected->cases.push_back({makeShallowList({"1"}), case_1});
    expected->cases.push_back({makeShallowList({"a", "b", "c"}), case_abc});
    parser.parseUnionDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseUnionTest4) {
    auto scanner = scannerFromString("union Kappa { case(1) { struct Keepo {}; }; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Union>("Kappa", result);
    auto case_1 = std::make_shared<Struct>("", expected);
    case_1->subtypes.push_back(std::make_shared<Struct>("Keepo", case_1));
    expected->cases.push_back({makeShallowList({"1"}), case_1});
    parser.parseUnionDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseUnionTest5) {
    auto scanner = scannerFromString("union Kappa { case(1) { struct Kappa {}; }; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Union>("Kappa", result);
    auto case_1 = std::make_shared<Struct>("", expected);
    case_1->subtypes.push_back(std::make_shared<Struct>("Kappa", case_1));
    expected->cases.push_back({makeShallowList({"1"}), case_1});
    parser.parseUnionDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_CASE(ParseUnionTest6) {
    auto scanner = scannerFromString("union() Kappa { default { int; }; };");
    Parser parser{scanner};
    auto result = std::make_shared<Struct>();
    auto expected = std::make_shared<Union>("Kappa", result);
    auto default_ = std::make_shared<Struct>("", expected);
    default_->declared.push_back({parser.Int, "", makeShallowList({"1"}), {makeShallowList({"32"})}});
    expected->default_case = default_;
    parser.parseUnionDeclaration(result);
    BOOST_CHECK_EQUAL(*expected, *result->subtypes.back());
}

BOOST_AUTO_TEST_SUITE_END();
