#include "tests_utility.h"

#include <memory>
#include <sstream>

Scanner scannerFromString(std::string in) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << in;
    return Scanner{std::move(ss)};
}

BinaryReader readerFromString(std::string in) {
    std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
    *ss << in;
    return BinaryReader{std::move(ss)};
}

ReferenceList makeShallowList(const std::vector<std::string>& names) {
    ReferenceList list;
    for (const auto& name : names)
        list.addElement({{}, name});
    return list;
}

DataArray makeDataArray(std::string identifier, int value) {
    return DataArray{identifier, "", 0, {}, nullptr, true, {}, value};
}

DataArray makeDataArray(std::string identifier) {
    return DataArray{identifier, "", 0, {}, nullptr, false, {}, 0};
}
