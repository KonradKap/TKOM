#include <boost/test/unit_test.hpp>

#include <sstream>

#include "filler/BinaryReader.h"

//using namespace std::string_literals;

BOOST_AUTO_TEST_SUITE(BinaryReaderTests);

BOOST_AUTO_TEST_CASE(ReadByte) {
    std::unique_ptr<std::stringstream> ss =
        std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    *ss << "\x01";
    BinaryReader reader{std::move(ss)};
    BOOST_CHECK_EQUAL(1, reader.read(8));
}

BOOST_AUTO_TEST_CASE(ReadUnalignedData) {
    std::unique_ptr<std::stringstream> ss =
        std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    *ss << "\xB0";
    BinaryReader reader{std::move(ss)};
    BOOST_CHECK_EQUAL(2, reader.read(2));
}

BOOST_AUTO_TEST_CASE(MultipleReads) {
    std::unique_ptr<std::stringstream> ss =
        std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    *ss << "\x28";
    BinaryReader reader{std::move(ss)};
    BOOST_CHECK_EQUAL(1, reader.read(3));
    BOOST_CHECK_EQUAL(2, reader.read(3));
}

BOOST_AUTO_TEST_CASE(ReadDifferentSizes) {
    std::unique_ptr<std::stringstream> ss =
        std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    *ss << "\xB8";
    BinaryReader reader{std::move(ss)};
    BOOST_CHECK_EQUAL(1, reader.read(1));
    BOOST_CHECK_EQUAL(3, reader.read(3));
    BOOST_CHECK_EQUAL(2, reader.read(2));
}

BOOST_AUTO_TEST_CASE(ReadZeroReturnsZero) {
    std::unique_ptr<std::stringstream> ss =
        std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    *ss << "\xFF";
    BinaryReader reader{std::move(ss)};
    std::ignore = reader.read(3);
    BOOST_CHECK_EQUAL(0, reader.read(0));
}

//BOOST_AUTO_TEST_CASE(FailingTest) {
//    std::unique_ptr<std::stringstream> ss =
//        std::make_unique<std::stringstream>(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
//    *ss << "\x0000007b"s;
//    BinaryReader reader{std::move(ss)};
//    BOOST_CHECK_EQUAL(123, reader.read(32));
//}

BOOST_AUTO_TEST_SUITE_END();
