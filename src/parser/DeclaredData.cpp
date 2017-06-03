#include "DeclaredData.h"

#include "Type.h"
#include "utility/print_container.h"

decltype(DeclaredData::defaultCount) DeclaredData::defaultCount{ReferenceList{}.addElement({{}, "1"})};

bool operator==(const DeclaredData& lhs, const DeclaredData& rhs) {
    return *lhs.type.lock() == *rhs.type.lock()
        and lhs.identifier == rhs.identifier
        and lhs.count == rhs.count
        and lhs.appliedArguments == rhs.appliedArguments;
}

bool operator!=(const DeclaredData& lhs, const DeclaredData& rhs) {
    return not (lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const DeclaredData& obj) {
    using namespace std::literals::string_literals;
    os << obj.type.lock()->type_name << ":"s << obj.identifier << obj.count;
    print(obj.appliedArguments, os);
    return os;
}

std::string DeclaredData::type_name() const {
    return type.lock()->type_name;
}
